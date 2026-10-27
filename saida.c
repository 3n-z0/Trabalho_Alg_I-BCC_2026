#include <stdio.h>
#include <string.h>
#include "saida.h"

/* Codigo movido de leitor_nasa_salvar: gravar nao depende do dataset, so
 * do cabecalho e da linha original de cada ITEM. */

typedef struct { FILE *arquivo; size_t gravados; bool ok; const char *quebra; } Escrita;

static bool escrever_linha(FILE *arquivo, const char *linha, const char *quebra) {
    if (!linha || fputs(linha, arquivo) == EOF) return false;
    size_t n = strlen(linha);
    /* O ultimo registro sem newline pode mudar de posicao na estrutura. */
    return n && linha[n-1] == '\n' ? true : fputs(quebra, arquivo) != EOF;
}

static void escrever_item(ITEM *item, void *ctx) {
    Escrita *e = ctx;
    if (!e->ok) return;
    e->ok = escrever_linha(e->arquivo, item_get_linha_original(item), e->quebra);
    if (e->ok) e->gravados++;
}

bool saida_gravar(const char *caminho, const char *cabecalho,
                  const ESTRUTURA *resultado, size_t top_k, size_t *gravados) {
    if (!caminho || !cabecalho || !resultado || !gravados) return false;
    *gravados = 0;
    FILE *arq = fopen(caminho, "wb");
    if (!arq) return false;
    const char *quebra = strstr(cabecalho, "\r\n") ? "\r\n" : "\n";
    Escrita e = {arq, 0, escrever_linha(arq, cabecalho, quebra), quebra};
    if (e.ok) estrutura_percorrer(resultado, top_k, escrever_item, &e);
    if (fclose(arq) != 0) e.ok = false;
    *gravados = e.gravados;
    return e.ok;
}
