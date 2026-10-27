#ifndef LEITOR_NASA_H
#define LEITOR_NASA_H
#include "item.h"
#include "leitor.h"

typedef struct leitor_nasa_ LEITOR_NASA;
LEITOR_NASA *leitor_nasa_abrir(const char *caminho);
/* 1: registro lido; 0: EOF; -1: CSV invalido ou erro de memoria/arquivo. */
int leitor_nasa_proximo(LEITOR_NASA *leitor, ITEM **item);
/* Cabecalho original do arquivo (com a quebra de linha), para a saida. */
const char *leitor_nasa_cabecalho(const LEITOR_NASA *leitor);
void leitor_nasa_fechar(LEITOR_NASA **leitor);

/* Este leitor na interface comum (leitor.h), registrado em leitor.c. */
extern const LEITOR_TIPO LEITOR_NASA_TIPO;
#endif
