#ifndef ITEM_STI_H
    #define ITEM_STI_H
    #include <stdbool.h>
    #include "item.h"

    /* Parte especifica do dataset STI - Sistema de Trafego Internacional
     * (Parte 2): movimentos migratorios por UF, tipo, classificacao e
     * nacionalidade. Junto com leitor_sti, e o unico modulo que conhece
     * essas colunas; o resto do programa usa a interface generica de item.h. */

    //operacoes do dataset, para item_definir_dataset() e item_criar();
    extern const ITEM_OPS ITEM_OPS_STI;

    //cria um ITEM da STI; textos copiados no tamanho exato (NULL vira "");
    //nacionalidade "" representa valor ausente;
    ITEM *item_sti_criar(
        const char *uf_atendimento,
        const char *tipo,
        const char *classificacao,
        const char *nacionalidade,
        double total
    );

//getters especificos; devolvem NULL/0 se o ITEM for NULL ou nao for da STI;
    const char *item_sti_get_uf_atendimento(const ITEM *item);
    const char *item_sti_get_tipo(const ITEM *item);
    const char *item_sti_get_classificacao(const ITEM *item);
    const char *item_sti_get_nacionalidade(const ITEM *item);
    double item_sti_get_total(const ITEM *item);
#endif
