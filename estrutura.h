#ifndef ESTRUTURA_H
#define ESTRUTURA_H
#include <stdbool.h>
#include <stddef.h>
#include "item.h"
#include "query.h"

typedef enum {
    RESULT_PILHA, RESULT_FILA, RESULT_LISTA_SEQ, RESULT_LISTA_ENC,
    RESULT_LISTA_CABECA, RESULT_LISTA_ORD, RESULT_LISTA_GEN, RESULT_LISTA_CRUZ
} TipoResultado;

typedef struct estrutura_ ESTRUTURA;
bool estrutura_tipo(const char *nome, TipoResultado *tipo);
/* QUERY e emprestada e deve existir ate o fim do uso da estrutura. */
ESTRUTURA *estrutura_criar(TipoResultado tipo, QUERY *query);
bool estrutura_inserir(ESTRUTURA *e, ITEM *item, double similaridade);
size_t estrutura_percorrer(const ESTRUTURA *e, size_t limite,
                          void (*visita)(ITEM *, void *), void *ctx);
/* Base: liberar_itens=true; resultado: false. Cada ITEM aparece uma vez na base. */
void estrutura_apagar(ESTRUTURA **e, bool liberar_itens);
#endif
