#include <stdio.h>
#include <stdlib.h>
#include "lista_seq.h"

#define CAPACIDADE_INICIAL 16

struct lista_seq {
    ITEM **itens;
    int tamanho;
    int capacidade;
};

LISTA_SEQ *lista_seq_criar(void) {
    LISTA_SEQ *l = (LISTA_SEQ *) malloc(sizeof(LISTA_SEQ));
    if (l != NULL) {
        l->itens = (ITEM **) malloc(CAPACIDADE_INICIAL * sizeof(ITEM *));
        if (l->itens == NULL) {
            free(l);
            return NULL;
        }
        l->tamanho = 0;
        l->capacidade = CAPACIDADE_INICIAL;
    }
    return l;
}

void lista_seq_apagar(LISTA_SEQ **l) {
    if (l == NULL || *l == NULL) return;
    free((*l)->itens); // Liberta o vetor de apontadores
    free(*l);
    *l = NULL;
}

bool lista_seq_inserir(LISTA_SEQ *l, ITEM *item) {
    if (l == NULL) return false;

    if (l->tamanho == l->capacidade) {
        int nova_cap = l->capacidade * 2;
        ITEM **novos_itens = (ITEM **) realloc(l->itens, nova_cap * sizeof(ITEM *));
        if (novos_itens == NULL) return false;

        l->itens = novos_itens;
        l->capacidade = nova_cap;
    }

    l->itens[l->tamanho] = item;
    l->tamanho++;
    return true;
}

ITEM *lista_seq_obter(const LISTA_SEQ *l, int pos) {
    if (l == NULL || pos < 0 || pos >= l->tamanho) return NULL;
    return l->itens[pos];
}

ITEM *lista_seq_remover(LISTA_SEQ *l, int pos) {
    if (l == NULL || pos < 0 || pos >= l->tamanho) return NULL;

    ITEM *removido = l->itens[pos];

    for (int i = pos; i < l->tamanho - 1; i++) {
        l->itens[i] = l->itens[i + 1];
    }

    l->tamanho--;
    return removido;
}

bool lista_seq_vazia(const LISTA_SEQ *l) {
    if (l == NULL) return true;
    return l->tamanho == 0;
}

int lista_seq_tamanho(const LISTA_SEQ *l) {
    if (l == NULL) return 0;
    return l->tamanho;
}

size_t lista_seq_percorrer_limite(const LISTA_SEQ *l, size_t limite,
                                void (*visita)(ITEM *item, void *ctx), void *ctx) {
    if (l == NULL || visita == NULL) return 0;

    size_t visitados = 0;
    for (int i = 0; i < l->tamanho; i++) {
        if (limite != 0 && visitados >= limite) break;
        visita(l->itens[i], ctx);
        visitados++;
    }
    return visitados;
}

void lista_seq_percorrer(const LISTA_SEQ *l,
                        void (*visita)(ITEM *item, void *ctx), void *ctx) {
    (void) lista_seq_percorrer_limite(l, 0, visita, ctx);
}