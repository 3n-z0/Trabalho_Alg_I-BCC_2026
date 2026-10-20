#include <stdio.h>
#include <stdlib.h>
#include "pilha.h"

typedef struct no_st {
    ITEM *item;
    struct no_st *proximo;
} NO;

struct pilha {
    NO *topo;
    int tamanho;
};

PILHA *pilha_criar(void) {
    PILHA *p = (PILHA *) malloc(sizeof(PILHA));
    if (p != NULL) {
        p->topo = NULL;
        p->tamanho = 0;
    }
    return p;
}

void pilha_apagar(PILHA **p) {
    if (p == NULL || *p == NULL) return;

    NO *atual = (*p)->topo;
    while (atual != NULL) {
        NO *temp = atual;
        atual = atual->proximo;
        free(temp); // Liberta o nó da pilha, sem eliminar o ITEM
    }
    free(*p);
    *p = NULL;
}

bool pilha_empilhar(PILHA *p, ITEM *item) {
    if (p == NULL) return false;

    NO *novo = (NO *) malloc(sizeof(NO));
    if (novo == NULL) return false;

    novo->item = item;
    novo->proximo = p->topo;
    p->topo = novo;
    p->tamanho++;
    return true;
}

ITEM *pilha_desempilhar(PILHA *p) {
    if (p == NULL || p->topo == NULL) return NULL;

    NO *removido = p->topo;
    ITEM *item = removido->item;

    p->topo = removido->proximo;
    free(removido);
    p->tamanho--;

    return item;
}

ITEM *pilha_topo(const PILHA *p) {
    if (p == NULL || p->topo == NULL) return NULL;
    return p->topo->item;
}

bool pilha_vazia(const PILHA *p) {
    if (p == NULL) return true;
    return p->tamanho == 0;
}

int pilha_tamanho(const PILHA *p) {
    if (p == NULL) return 0;
    return p->tamanho;
}   