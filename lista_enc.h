#ifndef LISTA_ENC_H
 #define LISTA_ENC_H

 #include <stdbool.h> 
 #include "item.h"

 typedef struct lista_enc LISTA_ENC;

 LISTA_ENC *lista_enc_criar(void);
 void lista_enc_apagar(LISTA_ENC **l);
 bool lista_enc_inserir(LISTA_ENC *l, ITEM *item);
 ITEM *lista_enc_obter(const LISTA_ENC *l, int pos);
 ITEM *lista_enc_remover(LISTA_ENC *l, int pos);
 bool lista_enc_vazia(const LISTA_ENC *l);
 int lista_enc_tamanho(const LISTA_ENC *l);
#endif
