#ifndef LISTA_CRUZ_H
    #define LISTA_CRUZ_H

    #include <stdbool.h>
    #include "item.h"

    typedef struct lista_cruz_ LISTA_CRUZ;

    LISTA_CRUZ *lista_cruz_criar(void);
    void lista_cruz_apagar(LISTA_CRUZ **lc);

    bool lista_cruz_inserir(LISTA_CRUZ *lc, ITEM *item, const char *lin, const char *col);
    void lista_cruz_percorrer(const LISTA_CRUZ *lc, void (*visita)(ITEM *item, void *ctx), void *ctx);
#endif