#ifndef BUSCA_H
    #define BUSCA_H

    #include <stdbool.h>
    #include <stddef.h>
    #include "query.h"

    typedef enum {
        RESULT_PILHA,
        RESULT_FILA,
        RESULT_LISTA_SEQ,
        RESULT_LISTA_ENC,
        RESULT_LISTA_CABECA,
        RESULT_LISTA_ORD,
        RESULT_LISTA_GEN,
        RESULT_LISTA_CRUZ
    } TipoResultado;

    /*
     * Percorre a estrutura base (tipo_base) e insere em resultado (tipo_res)
     * cada ITEM aceito pela query. As estruturas ja devem estar criadas.
     * Para RESULT_LISTA_ORD, a lista deve ter sido criada como
     * LISTA_ORD_NUMERICA quando ORDER for similarity ou campo numerico,
     * e LISTA_ORD_TEXTUAL caso contrario.
     *
     * Em *aceitos (se nao for NULL) grava quantos ITEMs foram inseridos com
     * sucesso no resultado. Retorna false se algum argumento for invalido,
     * se a query nao tiver ORDER quando o resultado for lista_ord, ou se
     * alguma insercao falhar.
     */
    bool buscar(void *base, TipoResultado tipo_base, QUERY *query,
                void *resultado, TipoResultado tipo_res, size_t *aceitos);

#endif