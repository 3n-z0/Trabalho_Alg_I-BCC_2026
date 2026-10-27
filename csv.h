#ifndef CSV_H
#define CSV_H
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

/* Leitura de CSV compartilhada pelos leitores (nasa, sti...): o que muda
 * entre datasets (delimitador e numero de colunas) vem por parametro. */

/* Le um registro completo, ja alocado em *linha (o chamador libera).
 * Campos entre aspas podem conter o delimitador e quebras de linha.
 * A quebra de linha final (LF ou CRLF) fica na string.
 * Retorna 1 (registro lido), 0 (fim do arquivo) ou -1 (erro de leitura,
 * memoria, byte nulo ou aspas nao fechadas). */
int csv_ler_registro(FILE *arquivo, char **linha);

/* Separa a linha em exatamente n_campos campos, no proprio buffer: cada
 * campos[i] aponta para dentro de linha. Preserva campos vazios, remove as
 * aspas externas e decodifica "" como ". Falso se o numero de campos for
 * diferente de n_campos ou se as aspas estiverem mal formadas. */
bool csv_separar(char *linha, char delimitador, char *campos[], size_t n_campos);

#endif
