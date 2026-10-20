#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "item.h"
#include "lista_cabeca.h"

typedef struct no_{
    ITEM *item;
    struct no_ *prox;
}NO;

struct lista_cabeca_{
    NO* cabeca;
    NO* fim;
    int tam;
};

LISTA_CABECA *lista_cabeca_criar(void){
    LISTA_CABECA *lista_cabeca = (LISTA_CABECA*)malloc(sizeof(LISTA_CABECA));
    if(lista_cabeca == NULL){
        return NULL;
    }

    lista_cabeca->cabeca = (NO*)malloc(sizeof(NO));
    if(lista_cabeca->cabeca == NULL){
        free(lista_cabeca);
        return NULL;
    }
    lista_cabeca->cabeca->prox = NULL;
    lista_cabeca->cabeca->item = NULL;

    lista_cabeca->fim = lista_cabeca->cabeca;
    lista_cabeca->tam = 0;

    return lista_cabeca;
}

void lista_cabeca_apagar(LISTA_CABECA **lista_cabeca){
    if(lista_cabeca == NULL || *lista_cabeca == NULL){
        return;
    }
    NO* atual = (*lista_cabeca)->cabeca;
    NO* apaga = NULL;

    while(atual != NULL){
        apaga = atual;
        atual = atual->prox;

        item_apagar(&apaga->item);
        free(apaga);
        apaga = NULL;
    }

    free(*lista_cabeca);
    *lista_cabeca = NULL;

    return;
}

bool lista_cabeca_inserir_fim(LISTA_CABECA *lista_cabeca, ITEM *item){
    if(lista_cabeca == NULL || item == NULL){
        return false;
    }

    NO* no = (NO*)malloc(sizeof(NO));
    if(no == NULL){
        return false;
    }
    no->item = item;
    no->prox = NULL;

    lista_cabeca->tam++;
    lista_cabeca->fim->prox = no;
    lista_cabeca->fim = no;
    return true;
}

bool lista_cabeca_inserir_inicio(LISTA_CABECA *lista_cabeca, ITEM *item){
     if(lista_cabeca == NULL || item == NULL){
        return false;
    }

    NO* no = (NO*)malloc(sizeof(NO));
    if(no == NULL){
        return false;
    }
    no->item = item;

    if(lista_cabeca->tam == 0){
        lista_cabeca->fim = no;
    }

    lista_cabeca->tam++;
    no->prox = lista_cabeca->cabeca->prox;
    lista_cabeca->cabeca->prox = no;
    return true;
}

ITEM *lista_cabeca_buscar_item(const LISTA_CABECA *lista_cabeca, CRITERIO_BUSCA criterio, void *contexto){
    if(lista_cabeca == NULL || criterio == NULL || contexto == NULL){
        return NULL;
    }
    NO *atual = lista_cabeca->cabeca->prox;
    while(atual != NULL){
        if(criterio(atual->item, contexto)){
            return atual->item;
        }
        atual = atual->prox;
    }
    return NULL;
}

