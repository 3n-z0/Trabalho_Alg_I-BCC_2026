#ifndef PILHA_H
#define PILHA_H

#include <stdbool.h>
#include <stddef.h>
#include "item.h"

typedef struct pilha PILHA;

PILHA *pilha_criar(void);
void pilha_apagar(PILHA **p);
bool pilha_empilhar(PILHA *p, ITEM *item);
ITEM *pilha_desempilhar(PILHA *p);
ITEM *pilha_topo(const PILHA *p);
bool pilha_vazia(const PILHA *p);
int pilha_tamanho(const PILHA *p);

/* Percorre do topo a base (ordem LIFO, a mesma de desempilhar), sem
 * desempilhar: a pilha continua intacta. O callback nao deve empilhar,
 * desempilhar nem apagar a pilha durante o percurso. */
void pilha_percorrer(const PILHA *p,
                    void (*visita)(ITEM *item, void *ctx), void *ctx);

/* Mesmo percurso, limitado a top-k itens; limite 0 visita todos.
 * Retorna a quantidade de ITEMs visitados. Nao remove registros da pilha. */
size_t pilha_percorrer_limite(const PILHA *p, size_t limite,
                            void (*visita)(ITEM *item, void *ctx), void *ctx);

#endif