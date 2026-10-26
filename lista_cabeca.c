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

        free(apaga);
        apaga = NULL;
    }

    free(*lista_cabeca);
    *lista_cabeca = NULL;
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

size_t lista_cabeca_percorrer_limite(const LISTA_CABECA *lc, size_t limite,
                                     void (*visita)(ITEM *item, void *ctx), void *ctx){
    if(lc == NULL || lc->cabeca == NULL || visita == NULL) return 0;

    size_t visitados = 0;
    NO *atual = lc->cabeca->prox; //pula o no cabeca, que nao guarda registro;
    while(atual != NULL){
        if(limite != 0 && visitados >= limite) break;
        visita(atual->item, ctx);
        visitados++;
        atual = atual->prox;
    }
    return visitados;
}

void lista_cabeca_percorrer(const LISTA_CABECA *lc, void (*visita)(ITEM *item, void *ctx), void *ctx){
    (void) lista_cabeca_percorrer_limite(lc, 0, visita, ctx);
}

/* Devolve o no ANTERIOR ao primeiro ITEM que satisfaz o criterio, ou o
 * ultimo no se nenhum satisfizer. Como a busca parte do no cabeca, sempre
 * existe um anterior, inclusive para o primeiro registro: o laco nao testa
 * lista vazia nem trata o inicio como caso especial, e uma remocao por
 * criterio poderia religar anterior->prox direto a partir deste no. */
static NO *anterior_de(const LISTA_CABECA *l, bool (*criterio)(const ITEM *item, void *ctx), void *ctx){
    NO *anterior = l->cabeca;
    while(anterior->prox != NULL && !criterio(anterior->prox->item, ctx)){
        anterior = anterior->prox;
    }
    return anterior;
}

ITEM *lista_cabeca_buscar(const LISTA_CABECA *l, bool (*criterio)(const ITEM *item, void *ctx), void *ctx){
    if (l == NULL || criterio == NULL) return NULL;

    NO *anterior = anterior_de(l, criterio, ctx);
    return anterior->prox != NULL ? anterior->prox->item : NULL;
}

ITEM *lista_cabeca_obter(const LISTA_CABECA *l, int pos){
    if (l == NULL || pos < 0 || pos >= l->tam) return NULL;

    NO *atual = l->cabeca->prox;
    for (int i = 0; i < pos; i++) {
        atual = atual->prox;
    }
    return atual->item;
}

int lista_cabeca_tamanho(const LISTA_CABECA *l){
    if (l == NULL) return 0;
    return l->tam; //o no cabeca nao e contado;
}

bool lista_cabeca_vazia(const LISTA_CABECA *l){
    //com no cabeca, vazia = cabeca sem sucessor;
    return l == NULL || l->cabeca->prox == NULL;
}

ITEM *lista_cabeca_remover(LISTA_CABECA *lc, int pos){
    if (lc == NULL || pos < 0 || pos >= lc->tam) return NULL;

    //comeca no no cabeca: remover o primeiro nao precisa de caso especial;
    NO *anterior = lc->cabeca;
    for (int i = 0; i < pos; i++) {
        anterior = anterior->prox;
    }
    NO *atual = anterior->prox;

    anterior->prox = atual->prox;
    if (atual == lc->fim) lc->fim = anterior; //removeu o ultimo;
    lc->tam--;

    ITEM *item = atual->item;
    free(atual);
    atual = NULL;

    return item;
}

