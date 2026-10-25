#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

typedef struct no_st {
    ITEM *item;
    struct no_st *proximo;
} NO;

struct fila {
    NO *inicio;
    NO *fim;
    int tamanho;
};

FILA *fila_criar(void) {
    FILA *f = (FILA *) malloc(sizeof(FILA));
    if (f != NULL) {
        f->inicio = NULL;
        f->fim = NULL;
        f->tamanho = 0;
    }
    return f;
}

void fila_apagar(FILA **f) {
    if (f == NULL || *f == NULL) return;

    NO *atual = (*f)->inicio;
    while (atual != NULL) {
        NO *temp = atual;
        atual = atual->proximo;
        free(temp); // Liberta apenas o nó interno
    }
    free(*f);
    *f = NULL;
}

bool fila_enfileirar(FILA *f, ITEM *item) {
    if (f == NULL) return false;

    NO *novo = (NO *) malloc(sizeof(NO));
    if (novo == NULL) return false;

    novo->item = item;
    novo->proximo = NULL;

    if (f->fim != NULL) {
        f->fim->proximo = novo;
    } else {
        f->inicio = novo;
    }
    f->fim = novo;
    f->tamanho++;
    return true;
}

ITEM *fila_desenfileirar(FILA *f) {
    if (f == NULL || f->inicio == NULL) return NULL;

    NO *removido = f->inicio;
    ITEM *item = removido->item;

    f->inicio = removido->proximo;
    if (f->inicio == NULL) {
        f->fim = NULL;
    }

    free(removido);
    f->tamanho--;
    return item;
}

ITEM *fila_inicio(const FILA *f) {
    if (f == NULL || f->inicio == NULL) return NULL;
    return f->inicio->item;
}

bool fila_vazia(const FILA *f) {
    if (f == NULL) return true;
    return f->tamanho == 0;
}

int fila_tamanho(const FILA *f) {
    if (f == NULL) return 0;
    return f->tamanho;
}

size_t fila_percorrer_limite(const FILA *f, size_t limite,
                           void (*visita)(ITEM *item, void *ctx), void *ctx) {
    if (f == NULL || visita == NULL) return 0;

    size_t visitados = 0;
    for (NO *atual = f->inicio; atual != NULL; atual = atual->proximo) {
        if (limite != 0 && visitados >= limite) break;
        visita(atual->item, ctx);
        visitados++;
    }
    return visitados;
}

void fila_percorrer(const FILA *f,
                   void (*visita)(ITEM *item, void *ctx), void *ctx) {
    (void) fila_percorrer_limite(f, 0, visita, ctx);
}