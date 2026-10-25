#ifndef LISTA_CRUZ_H
    #define LISTA_CRUZ_H

    #include <stdbool.h>
    #include <stddef.h>
    #include "item.h"

    typedef struct lista_cruz_ LISTA_CRUZ;

    LISTA_CRUZ *lista_cruz_criar(void);
    void lista_cruz_apagar(LISTA_CRUZ **lc);

    bool lista_cruz_inserir(LISTA_CRUZ *lc, ITEM *item, const char *lin, const char *col);

    /* Percorre linha a linha (pela direita), na ordem de criacao das linhas.
     * Cada celula e visitada uma unica vez. O callback nao deve inserir
     * celulas nem apagar a lista durante o percurso. */
    void lista_cruz_percorrer(const LISTA_CRUZ *lc, void (*visita)(ITEM *item, void *ctx), void *ctx);

    /* Mesmo percurso, limitado a top-k itens; limite 0 visita todos.
     * Retorna a quantidade de ITEMs visitados. Nao remove registros da lista. */
    size_t lista_cruz_percorrer_limite(const LISTA_CRUZ *lc, size_t limite,
                                       void (*visita)(ITEM *item, void *ctx), void *ctx);
#endif