#ifndef LISTA_GENERALIZADA_H
    #define LISTA_GENERALIZADA_H
    #include "item.h"
    #include <stdbool.h>

    typedef struct lista_gen_ LISTA_G;

    LISTA_G *lista_gen_criar(void);
    bool lista_gen_inserir_atomo(LISTA_G *lista, ITEM *item);
    LISTA_G *lista_gen_inserir_sublista(LISTA_G *lista);
    void lista_gen_apagar(LISTA_G **lista);
    int lista_gen_get_profundidade(LISTA_G *lista);

#endif