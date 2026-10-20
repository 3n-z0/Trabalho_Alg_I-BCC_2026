#ifndef LISTA_CABECA_H
    #define LISTA_CABECA_H
    #include <stdbool.h>
    #include "item.h"

    typedef struct lista_cabeca_ LISTA_CABECA;

    typedef bool (*CRITERIO_BUSCA)(const ITEM *item, void *contexto);

    LISTA_CABECA *lista_cabeca_criar(void);
    void lista_cabeca_apagar(LISTA_CABECA **lista_cabeca);
    bool lista_cabeca_inserir_fim(LISTA_CABECA *lista_cabeca, ITEM *item);
    bool lista_cabeca_inserir_inicio(LISTA_CABECA *lista_cabeca, ITEM *item);
    ITEM *lista_cabeca_buscar_item(const LISTA_CABECA *lista_cabeca, CRITERIO_BUSCA criterio, void *contexto);

#endif