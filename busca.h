#ifndef QUERY_H
    #define QUERY_H

    #include <stdbool.h>
    #include "item.h"

    typedef struct query_ QUERY;

    QUERY *query_criar(const char *dados_query);

    bool query_avaliar(QUERY *query, ITEM *item, float *sim);

    void query_apagar(QUERY **query);

#endif