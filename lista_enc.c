#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lista_enc.h"

typedef struct no_st {
    ITEM *item;
    struct no_st *proximo;
} NO;

struct lista_enc {
    NO *inicio;
    NO *fim;
    int tamanho;
};

LISTA_ENC *lista_enc_criar(void) {
    LISTA_ENC *l = (LISTA_ENC *) malloc(sizeof(LISTA_ENC));
    if (l != NULL) {
        l->inicio = NULL;
        l->fim = NULL;
        l->tamanho = 0;
    }
    return l;
}

void lista_enc_apagar(LISTA_ENC **l) {
    if (l == NULL || *l == NULL) return;

    NO *atual = (*l)->inicio;
    while (atual != NULL) {
        NO *temp = atual;
        atual = atual->proximo;
        free(temp);
    }
    free(*l);
    *l = NULL;
}

bool lista_enc_inserir(LISTA_ENC *l, ITEM *item) {
    if (l == NULL) return false;

    NO *novo = (NO *) malloc(sizeof(NO));
    if (novo == NULL) return false;

    novo->item = item;
    novo->proximo = NULL;

    if (l->fim != NULL) {
        l->fim->proximo = novo;
    } else {
        l->inicio = novo;
    }
    l->fim = novo;
    l->tamanho++;
    return true;
}

ITEM *lista_enc_obter(const LISTA_ENC *l, int pos) {
    if (l == NULL || pos < 0 || pos >= l->tamanho) return NULL;

    NO *atual = l->inicio;
    for (int i = 0; i < pos; i++) {
        atual = atual->proximo;
    }
    return atual->item;
}

ITEM *lista_enc_remover(LISTA_ENC *l, int pos) {
    if (l == NULL || pos < 0 || pos >= l->tamanho) return NULL;

    NO *removido = NULL;

    if (pos == 0) {
        removido = l->inicio;
        l->inicio = l->inicio->proximo;
        if (l->inicio == NULL) {
            l->fim = NULL;
        }
    } else {
        NO *anterior = l->inicio;
        for (int i = 0; i < pos - 1; i++) {
            anterior = anterior->proximo;
        }
        removido = anterior->proximo;
        anterior->proximo = removido->proximo;

        if (removido == l->fim) {
            l->fim = anterior;
        }
    }

    ITEM *item = removido->item;
    free(removido);
    l->tamanho--;

    return item;
}

bool lista_enc_vazia(const LISTA_ENC *l) {
    if (l == NULL) return true;
    return l->tamanho == 0;
}

int lista_enc_tamanho(const LISTA_ENC *l) {
    if (l == NULL) return 0;
    return l->tamanho;
}

size_t lista_enc_percorrer_limite(const LISTA_ENC *l, size_t limite,
                                void (*visita)(ITEM *item, void *ctx), void *ctx) {
    if (l == NULL || visita == NULL) return 0;

    size_t visitados = 0;
    for (NO *atual = l->inicio; atual != NULL; atual = atual->proximo) {
        if (limite != 0 && visitados >= limite) break;
        visita(atual->item, ctx);
        visitados++;
    }
    return visitados;
}

void lista_enc_percorrer(const LISTA_ENC *l,
                        void (*visita)(ITEM *item, void *ctx), void *ctx) {
    (void) lista_enc_percorrer_limite(l, 0, visita, ctx);
}