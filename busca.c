#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "item.h"
#include "query.h"
#include "fila.h"
#include "pilha.h"
#include "lista_seq.h"
#include "lista_cabeca.h"


typedef struct{
    QUERY *query;
    void *estrutura_resultado;
    const char *tipo_resultado;
    int item_obtidos;
}ContextoBusca;

static void processa_item(ITEM *item, void *ctx){
    ContextoBusca *c = (ContextoBusca*) ctx;
    double sim = 0.0;
}

static void destruir_item_cb(ITEM *item, void *ctx) {
    (void) ctx; //ignora o contexto não utilizado
    item_apagar(&item); //destrói o ITEM
}
