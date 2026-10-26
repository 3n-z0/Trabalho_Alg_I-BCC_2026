#ifndef LEITOR_NASA_H
#define LEITOR_NASA_H
#include "estrutura.h"

typedef struct leitor_nasa_ LEITOR_NASA;
LEITOR_NASA *leitor_nasa_abrir(const char *caminho);
/* 1: registro lido; 0: EOF; -1: CSV invalido ou erro de memoria/arquivo. */
int leitor_nasa_proximo(LEITOR_NASA *leitor, ITEM **item);
void leitor_nasa_fechar(LEITOR_NASA **leitor);
bool leitor_nasa_salvar(const LEITOR_NASA *leitor, const char *caminho,
                       const ESTRUTURA *resultado, size_t top_k, size_t *gravados);
#endif
