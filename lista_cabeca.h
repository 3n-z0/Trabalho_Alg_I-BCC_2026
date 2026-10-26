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

    /* Busca o primeiro ITEM para o qual criterio(item, ctx) devolve true,
     * partindo do no cabeca; NULL se nenhum satisfizer. O ITEM nao e removido.
     * Custo O(n). Ex.: criterio que compara item_get_asteroid_designation. */
    ITEM *lista_cabeca_buscar(const LISTA_CABECA *l, bool (*criterio)(const ITEM *item, void *ctx), void *ctx);

    /* ITEM na posicao pos (0 = primeiro registro, apos o no cabeca); NULL se invalida. */
    ITEM *lista_cabeca_obter(const LISTA_CABECA *l, int pos);

    /* Remove a posicao pos sem liberar o ITEM; parte do no cabeca. */
    ITEM *lista_cabeca_remover(LISTA_CABECA *l, int pos);

    /* Quantidade de registros (o no cabeca nao conta). */
    int lista_cabeca_tamanho(const LISTA_CABECA *l);
    bool lista_cabeca_vazia(const LISTA_CABECA *l);

    /* Percorre a partir de cabeca->prox: o no cabeca nunca e visitado.
     * O callback nao deve inserir/remover nos nem apagar a lista durante o percurso. */
    void lista_cabeca_percorrer(const LISTA_CABECA *lc, void (*visita)(ITEM *item, void *ctx), void *ctx);

    /* Mesmo percurso, limitado a top-k itens; limite 0 visita todos.
     * Retorna a quantidade de ITEMs visitados. Nao remove registros da lista. */
    size_t lista_cabeca_percorrer_limite(const LISTA_CABECA *lc, size_t limite,
                                         void (*visita)(ITEM *item, void *ctx), void *ctx);
#endif