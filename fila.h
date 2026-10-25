#ifndef FILA_H
#define FILA_H

#include <stdbool.h>
#include <stddef.h>
#include "item.h"

typedef struct fila FILA;

FILA *fila_criar(void);
void fila_apagar(FILA **f);
bool fila_enfileirar(FILA *f, ITEM *item);
ITEM *fila_desenfileirar(FILA *f);
ITEM *fila_inicio(const FILA *f);
bool fila_vazia(const FILA *f);
int fila_tamanho(const FILA *f);

/* Percorre do inicio ao fim (ordem FIFO, a mesma de desenfileirar), sem
 * desenfileirar: a fila continua intacta. O callback nao deve enfileirar,
 * desenfileirar nem apagar a fila durante o percurso. */
void fila_percorrer(const FILA *f,
                   void (*visita)(ITEM *item, void *ctx), void *ctx);

/* Mesmo percurso, limitado a top-k itens; limite 0 visita todos.
 * Retorna a quantidade de ITEMs visitados. Nao remove registros da fila. */
size_t fila_percorrer_limite(const FILA *f, size_t limite,
                           void (*visita)(ITEM *item, void *ctx), void *ctx);

#endif