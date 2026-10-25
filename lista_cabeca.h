#ifndef LISTA_CABECA_H
    #define LISTA_CABECA_H
    #include <stdbool.h>
    #include <stddef.h>
    #include "item.h"

    typedef struct lista_cabeca_ LISTA_CABECA;

    LISTA_CABECA *lista_cabeca_criar(void);
    void lista_cabeca_apagar(LISTA_CABECA **lista_cabeca);
    bool lista_cabeca_inserir_fim(LISTA_CABECA *lista_cabeca, ITEM *item);
    bool lista_cabeca_inserir_inicio(LISTA_CABECA *lista_cabeca, ITEM *item);
    ITEM *lista_cabeca_buscar(const LISTA_CABECA *l, int pos);

    /* Percorre a partir de cabeca->prox: o no cabeca nunca e visitado.
     * O callback nao deve inserir/remover nos nem apagar a lista durante o percurso. */
    void lista_cabeca_percorrer(const LISTA_CABECA *lc, void (*visita)(ITEM *item, void *ctx), void *ctx);

    /* Mesmo percurso, limitado a top-k itens; limite 0 visita todos.
     * Retorna a quantidade de ITEMs visitados. Nao remove registros da lista. */
    size_t lista_cabeca_percorrer_limite(const LISTA_CABECA *lc, size_t limite,
                                         void (*visita)(ITEM *item, void *ctx), void *ctx);
#endif