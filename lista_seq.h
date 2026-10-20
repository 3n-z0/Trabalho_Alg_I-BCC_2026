#ifndef LISTA_SEQ_H
#define LISTA_SEQ_H

#include <stdbool.h>
#include "item.h"

typedef struct lista_seq LISTA_SEQ;

LISTA_SEQ *lista_seq_criar(void);
void lista_seq_apagar(LISTA_SEQ **l);
bool lista_seq_inserir(LISTA_SEQ *l, ITEM *item);
ITEM *lista_seq_obter(const LISTA_SEQ *l, int pos);
ITEM *lista_seq_remover(LISTA_SEQ *l, int pos);
bool lista_seq_vazia(const LISTA_SEQ *l);
int lista_seq_tamanho(const LISTA_SEQ *l);

#endif