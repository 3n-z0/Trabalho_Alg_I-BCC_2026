#ifndef LISTA_GENERALIZADA_H
#define LISTA_GENERALIZADA_H

#include <stdbool.h>
#include <stddef.h>
#include "item.h"

typedef struct lista_gen_ LISTA_GEN;

/*
 * A lista guarda ponteiros emprestados para ITEM: nunca libera os registros.
 * O cliente deve manter os ITEMs vivos enquanto forem usados pelas estruturas
 * e liberar cada registro uma unica vez ao encerrar o programa.
 */
LISTA_GEN *lista_gen_criar(void);
bool lista_gen_inserir_atomo(LISTA_GEN *lista, ITEM *item);

/* A sublista retornada pertence a lista. Nao a apague separadamente. */
LISTA_GEN *lista_gen_inserir_sublista(LISTA_GEN *lista);

/*
 * Organiza: lista -> grupo -> subgrupo -> ITEM.
 * Recebe os VALORES dos campos definidos em GROUP, ja extraidos pelo cliente;
 * por exemplo, "MONITOR" e "2026". Nao conhece campos do dataset nem QUERY.
 * Grupos existentes sao reutilizados. As chaves sao copiadas integralmente.
 * Strings vazias sao aceitas; ponteiros NULL sao rejeitados.
 * Em falha de alocacao, retorna false sem alterar a lista.
 */
bool lista_gen_inserir_agrupado(LISTA_GEN *lista, ITEM *item,
                              const char *grupo, const char *subgrupo);

/*
 * Percurso em profundidade, na ordem de insercao dos nos de cada nivel.
 * Visita somente atomos, sem passar sublistas para a funcao visita.
 * O callback nao deve inserir/remover nos nem apagar listas durante o percurso.
 */
void lista_gen_percorrer(const LISTA_GEN *lista,
                        void (*visita)(ITEM *item, void *ctx), void *ctx);

/* Mesmo percurso, limitado a top-k atomos; limite 0 visita todos.
 * Retorna a quantidade de ITEMs visitados. Nao remove registros da lista. */
size_t lista_gen_percorrer_limite(const LISTA_GEN *lista, size_t limite,
                                void (*visita)(ITEM *item, void *ctx), void *ctx);

/* Remove a primeira ocorrencia do ponteiro, sem liberar o ITEM.
 * Segue a ordem do percurso e preserva sublistas que ficarem vazias. */
bool lista_gen_remover(LISTA_GEN *lista, const ITEM *item);

/* Libera nos, chaves, sublistas e a lista; nao libera os ITEMs. */
void lista_gen_apagar(LISTA_GEN **lista);

/* Lista vazia: 0; lista so com atomos: 1; NULL: -1.
 * Cada nivel de sublista acrescenta 1 a profundidade do ramo. */
int lista_gen_get_profundidade(LISTA_GEN *lista);

#endif
