#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "item.h"
#include "query.h"
#include "fila.h"
#include "pilha.h"
#include "lista_seq.h"
#include "lista_cabeca.h"
#include "lista_cruz.h"
#include "busca.h"

typedef struct {
    QUERY *query;
    void *resultado;
    TipoResultado tipo_res;
} ContextoBusca;

static void inserir_resultado(ITEM *item, void *ctx, float sim){
    ContextoBusca *c = (ContextoBusca*) ctx;

    if(item == NULL || c == NULL) return;

    switch(c->tipo_res){
        case RESULT_PILHA:
            pilha_empilhar((PILHA*)c->resultado, item);
            break;
        case RESULT_FILA:
            fila_enfileirar((FILA*)c->resultado, item);
            break;
        case RESULT_LISTA_SEQ:
            lista_seq_inserir((LISTA_SEQ*)c->resultado, item);
            break;
        case RESULT_LISTA_CABECA:
            lista_cabeca_inserir_fim((LISTA_CABECA*)c->resultado, item);
            break;

        case RESULT_LISTA_CRUZ: {
                char val_dim1[64] = {0};
                char val_dim2[64] = {0};

                const char *campo1 = query_get_cross_campo1(c->query);
                const char *campo2 = query_get_cross_campo2(c->query);

                // Extrai os valores formatados como string diretamente do ITEM
                item_get_campo_str(item, campo1, val_dim1, sizeof(val_dim1));
                item_get_campo_str(item, campo2, val_dim2, sizeof(val_dim2));

                // Insere no TAD Lista Cruzada
                lista_cruz_inserir((LISTA_CRUZ *) c->resultado, item, val_dim1, val_dim2);
                break;
            }
        
        default: break;

    }
    

}

static void processa_item(ITEM *item, void *ctx){
    ContextoBusca *c = (ContextoBusca*) ctx;
    float sim = 0.0;

    if(item == NULL || c == NULL) return;

    if(query_avaliar(item, c->query, &sim)){
        inserir_resultado(item, c, sim);
    }
    return;
}

static void destruir_item(ITEM *item, void *ctx) {
    (void) ctx; //ignora o contexto não utilizado
    item_apagar(&item); //destrói o ITEM
}

void buscar(void *base, TipoResultado tipo_base, QUERY *query, void *resultado, TipoResultado tipo_res){
    ContextoBusca *c = (ContextoBusca*)malloc(sizeof(ContextoBusca));

    if(c == NULL) return;

    c->query = query;
    c->resultado = resultado;
    c->tipo_res = tipo_res;

    switch(tipo_base){
        case RESULT_PILHA: break;
        case RESULT_FILA: break;
        case RESULT_LISTA_SEQ: break;
        case RESULT_LISTA_ENC: break;
        case RESULT_LISTA_CABECA: break;
        case RESULT_LISTA_ORD: break;
        case RESULT_LISTA_GEN: break;
        case RESULT_LISTA_CRUZ: break;
        default: break;
    }

    free(c);
    c = NULL;
}
