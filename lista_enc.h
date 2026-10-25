#ifndef LISTA_ENC_H
 #define LISTA_ENC_H

 #include <stdbool.h>
 #include <stddef.h>
 #include "item.h"

 typedef struct lista_enc LISTA_ENC;

 LISTA_ENC *lista_enc_criar(void);
 void lista_enc_apagar(LISTA_ENC **l);
 bool lista_enc_inserir(LISTA_ENC *l, ITEM *item);
 ITEM *lista_enc_obter(const LISTA_ENC *l, int pos);
 ITEM *lista_enc_remover(LISTA_ENC *l, int pos);
 bool lista_enc_vazia(const LISTA_ENC *l);
 int lista_enc_tamanho(const LISTA_ENC *l);

 /* Percorre do inicio ao fim em O(n). Use no lugar de obter(i) em laco,
  * que custa O(n^2). O callback nao deve inserir/remover nos nem apagar
  * a lista durante o percurso. */
 void lista_enc_percorrer(const LISTA_ENC *l,
                         void (*visita)(ITEM *item, void *ctx), void *ctx);

 /* Mesmo percurso, limitado a top-k itens; limite 0 visita todos.
  * Retorna a quantidade de ITEMs visitados. Nao remove registros da lista. */
 size_t lista_enc_percorrer_limite(const LISTA_ENC *l, size_t limite,
                                 void (*visita)(ITEM *item, void *ctx), void *ctx);
#endif
