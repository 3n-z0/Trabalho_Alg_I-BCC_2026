#ifndef LEITOR_STI_H
#define LEITOR_STI_H
#include "item.h"
#include "leitor.h"

/* Leitor do CSV da STI (Policia Federal): separador ';', Latin-1, CRLF e
 * cabecalho UF_ATENDIMENTO;TIPO;CLASSIFICACAO;NACIONALIDADE;TOTAL. Os textos
 * nao sao convertidos: a saida usa a linha original, no mesmo formato. */
typedef struct leitor_sti_ LEITOR_STI;
LEITOR_STI *leitor_sti_abrir(const char *caminho);
/* 1: registro lido; 0: EOF; -1: CSV invalido ou erro de memoria/arquivo. */
int leitor_sti_proximo(LEITOR_STI *leitor, ITEM **item);
/* Cabecalho original do arquivo (com a quebra de linha), para a saida. */
const char *leitor_sti_cabecalho(const LEITOR_STI *leitor);
void leitor_sti_fechar(LEITOR_STI **leitor);

/* Este leitor na interface comum (leitor.h), registrado em leitor.c. */
extern const LEITOR_TIPO LEITOR_STI_TIPO;
#endif
