#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "item.h"
#include "query.h"
#include "fila.h"
#include "pilha.h"
#include "lista_seq.h"
#include "lista_cabeca.h"
#include "lista_enc.h"
#include "lista_cruz.h"
#include "lista_gen.h"
#include "lista_ord.h"
#include "busca.h"

//Como a chave do ORDER e obtida; decidido uma unica vez em buscar();
typedef enum {
    ORD_SIMILARIDADE,
    ORD_NUMERICO,
    ORD_TEXTO
} ModoOrd;

typedef struct {
    QUERY *query;
    void *resultado;
    TipoResultado tipo_res;
    ModoOrd modo_ord;
    const char *campo_ord;
    size_t aceitos; //insercoes bem-sucedidas no resultado;
    bool erro;      //alguma insercao falhou;
} ContextoBusca;

static bool inserir_resultado(ITEM *item, void *ctx, double sim){
    ContextoBusca *c = (ContextoBusca*) ctx;

    if(item == NULL || c == NULL) return false;

    switch(c->tipo_res){
        case RESULT_PILHA:
            return pilha_empilhar((PILHA*)c->resultado, item);
        case RESULT_FILA:
            return fila_enfileirar((FILA*)c->resultado, item);
        case RESULT_LISTA_SEQ:
            return lista_seq_inserir((LISTA_SEQ*)c->resultado, item);
        case RESULT_LISTA_CABECA:
            return lista_cabeca_inserir_fim((LISTA_CABECA*)c->resultado, item);

        case RESULT_LISTA_CRUZ: {
                char val_dim1[64] = {0};
                char val_dim2[64] = {0};

                const char *campo1 = query_get_cross_campo1(c->query);
                const char *campo2 = query_get_cross_campo2(c->query);

                // Extrai os valores formatados como string diretamente do ITEM
                item_get_campo_str(item, campo1, val_dim1, sizeof(val_dim1));
                item_get_campo_str(item, campo2, val_dim2, sizeof(val_dim2));

                return lista_cruz_inserir((LISTA_CRUZ*) c->resultado, item, val_dim1, val_dim2);
            }
        case RESULT_LISTA_ENC:
            return lista_enc_inserir((LISTA_ENC*)c->resultado, item);

        case RESULT_LISTA_GEN: {
                char val_dim1[64] = {0};
                char val_dim2[64] = {0};

                const char *campo1 = query_get_group_campo1(c->query);
                const char *campo2 = query_get_group_campo2(c->query);

                // Extrai os valores formatados como string diretamente do ITEM
                item_get_campo_str(item, campo1, val_dim1, sizeof(val_dim1));
                item_get_campo_str(item, campo2, val_dim2, sizeof(val_dim2));

                return lista_gen_inserir_agrupado((LISTA_GEN*) c->resultado, item, val_dim1, val_dim2);
            }
        case RESULT_LISTA_ORD: {
                LISTA_ORD *ord = (LISTA_ORD*)c->resultado;

                switch(c->modo_ord){
                    case ORD_SIMILARIDADE:
                        return lista_ord_inserir_numero(ord, item, sim);
                    case ORD_NUMERICO:
                        return lista_ord_inserir_numero(ord, item, item_get_campo_num(item, c->campo_ord));
                    case ORD_TEXTO: {
                            char val_dim[64] = {0};
                            item_get_campo_str(item, c->campo_ord, val_dim, sizeof(val_dim));
                            return lista_ord_inserir_texto(ord, item, val_dim);
                        }
                }
                return false;
            }

        default: return false;

    }
}

static void processa_item(ITEM *item, void *ctx){
    ContextoBusca *c = (ContextoBusca*) ctx;
    double sim = 0.0;

    if(item == NULL || c == NULL) return;

    if(query_avaliar(item, c->query, &sim)){
        if(inserir_resultado(item, c, sim)){
            c->aceitos++;
        }
        else{
            c->erro = true;
        }
    }
    return;
}

static void destruir_item(ITEM *item, void *ctx) {
    (void) ctx; //ignora o contexto não utilizado
    item_apagar(&item); //destrói o ITEM
}

bool buscar(void *base, TipoResultado tipo_base, QUERY *query,
            void *resultado, TipoResultado tipo_res, size_t *aceitos){
    if(aceitos != NULL) *aceitos = 0;
    if(base == NULL || query == NULL || resultado == NULL) return false;

    ContextoBusca contexto = {0};
    ContextoBusca *c = &contexto;

    c->query = query;
    c->resultado = resultado;
    c->tipo_res = tipo_res;

    //decide uma unica vez como obter a chave do ORDER, em vez de a cada item;
    if(tipo_res == RESULT_LISTA_ORD){
        const char *campo = query_get_order_campo(query);
        if(campo == NULL || campo[0] == '\0') return false;

        c->campo_ord = campo;
        if(strcmp(campo, "similarity") == 0) c->modo_ord = ORD_SIMILARIDADE;
        else if(item_campo_eh_numerico(campo)) c->modo_ord = ORD_NUMERICO;
        else c->modo_ord = ORD_TEXTO;
    }

    switch(tipo_base){
        case RESULT_PILHA:
            pilha_percorrer((PILHA*)base, processa_item, c);
            break;
        case RESULT_FILA:
            fila_percorrer((FILA*)base, processa_item, c);
            break;
        case RESULT_LISTA_SEQ:
            lista_seq_percorrer((LISTA_SEQ*)base, processa_item, c);
            break;
        case RESULT_LISTA_ENC:
            lista_enc_percorrer((LISTA_ENC*)base, processa_item, c);
            break;
        case RESULT_LISTA_CABECA:
            lista_cabeca_percorrer((LISTA_CABECA*)base, processa_item, c);
            break;
        case RESULT_LISTA_ORD:
            lista_ord_percorrer((LISTA_ORD*)base, processa_item, c);
            break;
        case RESULT_LISTA_GEN:
            lista_gen_percorrer((LISTA_GEN*)base, processa_item, c);
            break;
        case RESULT_LISTA_CRUZ:
            lista_cruz_percorrer((LISTA_CRUZ*)base, processa_item, c);
            break;
        default: return false;
    }

    if(aceitos != NULL) *aceitos = c->aceitos;
    return !c->erro;
}
