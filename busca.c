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

                lista_cruz_inserir((LISTA_CRUZ*) c->resultado, item, val_dim1, val_dim2);
                break;
            }
        case RESULT_LISTA_ENC:
            lista_cabeca_inserir_fim((LISTA_ENC*)c->resultado, item);
            break;

        case RESULT_LISTA_GEN: {
                char val_dim1[64] = {0};
                char val_dim2[64] = {0};

                const char *campo1 = query_get_group_campo1(c->query);
                const char *campo2 = query_get_group_campo2(c->query);

                // Extrai os valores formatados como string diretamente do ITEM
                item_get_campo_str(item, campo1, val_dim1, sizeof(val_dim1));
                item_get_campo_str(item, campo2, val_dim2, sizeof(val_dim2));

                lista_gen_inserir_agrupado((LISTA_GEN*) c->resultado, item, val_dim1, val_dim2);
                break;
            }
        case RESULT_LISTA_ORD: {
                char val_dim[64] = {0};

                const char *campo = query_get_order_campo(c->query);

                if(strcmp(campo, "distance_au") == 0 || strcmp(campo, "velocity_km_s") == 0 || strcmp(campo, "risk_score") == 0 || strcmp(campo, "year") == 0|| strcmp(campo, "month") == 0 || strcmp(campo, "absolute_magnitude") == 0 || strcmp(campo, "panic_level") == 0 || strcmp(campo, "days_until_approach") == 0 || strcmp(campo, "sentry_torino_scale") == 0 || strcmp(campo, "sentry_palermo_scale") == 0 || strcmp(campo, "sentry_diameter_km") == 0){
                    lista_ord_inserir_numero((LISTA_ORD*)c->resultado, item, item_get_campo_num(item, campo));
                }
                else if(strcmp(campo, "similarity") == 0){
                    lista_ord_inserir_numero((LISTA_ORD*)c->resultado, item, sim);
                }
                else{
                    item_get_campo_str(item, campo, val_dim, sizeof(val_dim));

                    lista_ord_inserir_texto((LISTA_ORD*)c->resultado, item, val_dim);
                }
                
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
