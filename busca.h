#ifndef BUSCA_H
    #define BUSCA_H

    #include <stdbool.h>
    #include <stddef.h>
    #include "query.h"
    #include "estrutura.h" //TipoResultado e ESTRUTURA vem daqui;

    typedef struct {
        size_t encontrados; //ITEMs aceitos e inseridos com sucesso no resultado;
        double busca_ms;    //percurso da base + avaliacao da query;
        double insercao_ms; //somente as insercoes no resultado;
    } ESTATISTICAS_BUSCA;

    /*
     * Percorre toda a base e insere em resultado cada ITEM aceito pela query,
     * junto com sua similaridade. As duas estruturas ja devem estar criadas
     * com estrutura_criar(). Os ITEMs continuam pertencendo a base.
     *
     * est (se nao for NULL) recebe a contagem de aceitos e os tempos.
     * Retorna false se algum argumento for invalido ou se alguma insercao
     * no resultado falhar.
     */
    bool buscar(ESTRUTURA *base, QUERY *query, ESTRUTURA *resultado,
                ESTATISTICAS_BUSCA *est);

#endif
