#ifndef LISTA_CRUZ_H
    #define LISTA_CRUZ_H
    #include <stdbool.h>
    #include "item.h"

    typedef struct lista_cruz_ LISTA_CRUZ;
    LISTA_CRUZ *lista_cruz_criar(int nlinhas, int ncolunas);
    bool lista_cruz_inserir(LISTA_CRUZ *matriz, ITEM *item, int linha, int coluna);
    ITEM *lista_cruz_remover(LISTA_CRUZ *matriz, int linha, int coluna);
    void lista_cruz_apagar(LISTA_CRUZ **matriz);

#endif