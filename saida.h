#ifndef SAIDA_H
#define SAIDA_H
#include <stdbool.h>
#include <stddef.h>
#include "estrutura.h"

/* Grava o arquivo de --output no mesmo formato da entrada, para qualquer
 * dataset: o cabecalho original e, para cada um dos top_k primeiros ITEMs
 * na ordem de acesso do resultado, a sua linha original (top_k 0 = todos).
 * A quebra de linha segue a do cabecalho (CRLF ou LF). Em *gravados fica a
 * quantidade de registros escritos (sem contar o cabecalho). */
bool saida_gravar(const char *caminho, const char *cabecalho,
                  const ESTRUTURA *resultado, size_t top_k, size_t *gravados);

#endif
