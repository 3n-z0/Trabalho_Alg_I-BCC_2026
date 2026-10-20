#ifndef FILA_H
#define FILA_H

#include <stdbool.h>
#include "item.h"

typedef struct fila FILA;

FILA *fila_criar(void);
void fila_apagar(FILA **f);
bool fila_enfileirar(FILA *f, ITEM *item);
ITEM *fila_desenfileirar(FILA *f);
ITEM *fila_inicio(const FILA *f);
bool fila_vazia(const FILA *f);
int fila_tamanho(const FILA *f);

#endif