#ifndef LISTA_CRUZ_H
    #define LISTA_CRUZ_H

    #include <stdbool.h>
    #include <stddef.h>
    #include "item.h"

    typedef struct lista_cruz_ LISTA_CRUZ;

    LISTA_CRUZ *lista_cruz_criar(void);
    void lista_cruz_apagar(LISTA_CRUZ **lc);

    bool lista_cruz_inserir(LISTA_CRUZ *lc, ITEM *item, const char *lin, const char *col);

    /* Cada celula pertence a uma linha (chave lin) e a uma coluna (chave col)
     * ao mesmo tempo, sem duplicar o ITEM: os dois percursos abaixo visitam
     * os mesmos registros, em organizacoes diferentes. Em ambos, cada celula
     * e visitada uma unica vez, e o callback nao deve inserir celulas nem
     * apagar a lista durante o percurso. */

    /* Percorre linha a linha (pela direita), na ordem de criacao das linhas
     * (ex.: por threat_category). */
    void lista_cruz_percorrer_linhas(const LISTA_CRUZ *lc, void (*visita)(ITEM *item, void *ctx), void *ctx);

    /* Mesmo percurso, limitado a top-k itens; limite 0 visita todos.
     * Retorna a quantidade de ITEMs visitados. Nao remove registros da lista. */
    size_t lista_cruz_percorrer_linhas_limite(const LISTA_CRUZ *lc, size_t limite,
                                              void (*visita)(ITEM *item, void *ctx), void *ctx);

    /* Percorre coluna a coluna (para baixo), na ordem de criacao das colunas
     * (ex.: por year). */
    void lista_cruz_percorrer_colunas(const LISTA_CRUZ *lc, void (*visita)(ITEM *item, void *ctx), void *ctx);

    /* Mesmo percurso, limitado a top-k itens; limite 0 visita todos.
     * Retorna a quantidade de ITEMs visitados. Nao remove registros da lista. */
    size_t lista_cruz_percorrer_colunas_limite(const LISTA_CRUZ *lc, size_t limite,
                                               void (*visita)(ITEM *item, void *ctx), void *ctx);
#endif