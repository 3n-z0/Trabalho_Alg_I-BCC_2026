#ifndef LEITOR_H
#define LEITOR_H
#include <stdio.h>
#include "item.h"

/* Interface comum dos leitores de dataset (--reader). Cada leitor (nasa,
 * sti...) e o unico modulo, junto com o seu item_*, que conhece o formato
 * do arquivo; a main so usa estes ponteiros e nao cita nenhum dataset.
 * O "leitor" (void *) e o estado interno de cada um (arquivo aberto etc.). */
typedef struct {
    const char *nome;      /* valor de --reader, ex.: "nasa" */
    const ITEM_OPS *ops;   /* campos do dataset, para item_definir_dataset() */

    /* Abre o arquivo e confere o cabecalho; NULL se inacessivel ou invalido. */
    void *(*abrir)(const char *caminho);
    /* 1: registro lido em *item; 0: fim do arquivo; -1: registro invalido ou erro. */
    int (*proximo)(void *leitor, ITEM **item);
    /* Cabecalho original do arquivo, para a saida no mesmo formato. */
    const char *(*cabecalho)(const void *leitor);
    /* Fecha o arquivo e libera o estado (aceita NULL). */
    void (*fechar)(void *leitor);
} LEITOR_TIPO;

/* Leitor com esse nome, ou NULL se nao existir. */
const LEITOR_TIPO *leitor_tipo(const char *nome);

/* Escreve os nomes disponiveis separados por virgula (para mensagens). */
void leitor_listar(FILE *saida);

#endif
