#ifndef LISTA_ORD_H
#define LISTA_ORD_H

#include <stdbool.h>
#include <stddef.h>
#include "item.h"

typedef struct lista_ord_ LISTA_ORD;

typedef enum {
    LISTA_ORD_NUMERICA,
    LISTA_ORD_TEXTUAL
} LISTA_ORD_TIPO;

typedef enum {
    LISTA_ORD_ASC,
    LISTA_ORD_DESC
} LISTA_ORD_ORDEM;

/*
 * O cliente interpreta ORDER e escolhe o tipo e a direcao da ordenacao.
 * A lista recebe os valores da chave; nao conhece campos do dataset nem QUERY.
 * Os ITEMs sao emprestados e nunca sao liberados pela lista. Devem continuar
 * vivos enquanto forem acessados pelas estruturas e ser liberados uma unica
 * vez pelo cliente. As chaves, por sua vez, sao copiadas pela lista.
 */
LISTA_ORD *lista_ord_criar(LISTA_ORD_TIPO tipo, LISTA_ORD_ORDEM ordem);
void lista_ord_apagar(LISTA_ORD **lista);

/*
 * Insere diretamente na posicao correta: nao ha ordenacao posterior.
 * Empates preservam a ordem de insercao. O tipo deve corresponder ao da lista.
 * false indica argumento invalido ou falha de alocacao; a lista nao e alterada.
 * A chave fica como estava na insercao. Para atualiza-la, remova e reinsira.
 * Custo: O(n) no pior caso; inserir no fim usa O(1) comparacoes.
 * Comparar/copiar texto tambem depende do comprimento da chave.
 */
bool lista_ord_inserir_numero(LISTA_ORD *lista, ITEM *item, double chave);
bool lista_ord_inserir_texto(LISTA_ORD *lista, ITEM *item, const char *chave);

/* Chave numerica NaN ou chave textual NULL representa valor ausente.
 * Ausentes ficam no final tanto em ASC quanto em DESC; "" e texto valido.
 * Textos seguem strcmp (ordem lexicografica, sensivel a maiusculas/minusculas).
 * Numeros devem usar inserir_numero: os textos "10" e "2" nao sao numeros. */

ITEM *lista_ord_inicio(const LISTA_ORD *lista);

/* Retira o primeiro no em O(1) e devolve o ITEM, sem libera-lo. */
ITEM *lista_ord_retirar_primeiro(LISTA_ORD *lista);

/* Remove a primeira ocorrencia do ponteiro em O(n), sem liberar o ITEM. */
bool lista_ord_remover(LISTA_ORD *lista, const ITEM *item);

bool lista_ord_vazia(const LISTA_ORD *lista);
size_t lista_ord_tamanho(const LISTA_ORD *lista);

/* O callback recebe os ITEMs na ordem da lista e nao deve alterar seus nos
 * nem apagar a lista durante o percurso. O percurso nao remove registros. */
void lista_ord_percorrer(const LISTA_ORD *lista,
                        void (*visita)(ITEM *item, void *ctx), void *ctx);

/* Visita os primeiros top-k registros; limite 0 significa todos.
 * Retorna o numero visitado. Custo O(min(limite,n)), ou O(n) se limite for 0,
 * sem contar o trabalho executado pelo callback. */
size_t lista_ord_percorrer_limite(const LISTA_ORD *lista, size_t limite,
                                void (*visita)(ITEM *item, void *ctx), void *ctx);

#endif
