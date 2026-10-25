#ifndef LISTA_SEQ_H
#define LISTA_SEQ_H

#include <stdbool.h>
#include <stddef.h>
#include "item.h"

typedef struct lista_seq LISTA_SEQ;

LISTA_SEQ *lista_seq_criar(void);
void lista_seq_apagar(LISTA_SEQ **l);
bool lista_seq_inserir(LISTA_SEQ *l, ITEM *item);
ITEM *lista_seq_obter(const LISTA_SEQ *l, int pos);
ITEM *lista_seq_remover(LISTA_SEQ *l, int pos);
bool lista_seq_vazia(const LISTA_SEQ *l);
int lista_seq_tamanho(const LISTA_SEQ *l);

/* Percorre do indice 0 ao fim. O callback nao deve inserir/remover itens
 * nem apagar a lista durante o percurso. */
void lista_seq_percorrer(const LISTA_SEQ *l,
                        void (*visita)(ITEM *item, void *ctx), void *ctx);

/* Mesmo percurso, limitado a top-k itens; limite 0 visita todos.
 * Retorna a quantidade de ITEMs visitados. Nao remove registros da lista. */
size_t lista_seq_percorrer_limite(const LISTA_SEQ *l, size_t limite,
                                void (*visita)(ITEM *item, void *ctx), void *ctx);

#endif