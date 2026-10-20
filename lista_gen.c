#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "item.h"
#include "lista_gen.h"

typedef struct no_ NO;
typedef enum{
    TIPO_ATOMO,
    TIPO_SUBLISTA
}TIPO_NO;

struct lista_gen_{
    NO *inicio;
    int profundidade;
};

struct no_{
    TIPO_NO tipo;

    union 
    {
        ITEM *atomo;
        LISTA_G *sublista;
    }info;

    NO *prox;
};

LISTA_G *lista_gen_criar(void){
    LISTA_G *lista = (LISTA_G*)malloc(sizeof(LISTA_G));
    if(lista != NULL){
        lista->inicio = NULL;
        lista->profundidade = 0;
    }
    return lista;
}

static NO *lista_gen_fim(LISTA_G *lista){
    if(lista != NULL){
        NO *fim = lista->inicio;
        if(fim == NULL){
            return NULL;
        }
        while(fim->prox != NULL){
            fim = fim->prox;
        }
        return fim;
    }
    return NULL;
}

bool lista_gen_inserir_atomo(LISTA_G *lista, ITEM *item){
    if(lista != NULL && item != NULL){
        NO *no = (NO*)malloc(sizeof(NO)), *fim;
        
        if(no == NULL){
            return false;
        }

        no->info.atomo = item;
        no->prox = NULL;
        no->tipo = TIPO_ATOMO;

        fim = lista_gen_fim(lista);

        if(fim == NULL){
            lista->inicio = no;
        }
        else{
            fim->prox = no;
        }

        return true;
    }
    return false;
}

LISTA_G *lista_gen_inserir_sublista(LISTA_G *lista){
    if(lista != NULL){
        NO *no = (NO*)malloc(sizeof(NO)), *fim;
        
        if(no == NULL){
            return NULL;
        }

        no->info.sublista = lista_gen_criar();
        if(no->info.sublista == NULL){
            free(no);
            return NULL;
        }
        no->prox = NULL;
        no->tipo = TIPO_SUBLISTA;
        no->info.sublista->profundidade = lista->profundidade + 1;

        fim = lista_gen_fim(lista);

        if(fim == NULL){
            lista->inicio = no;
        }
        else{
            fim->prox = no;
        }
        
        return no->info.sublista;
    }
    return NULL;
}

static void atomo_apagar(NO **atomo){
    if(atomo != NULL && (*atomo) != NULL){
        item_apagar(&(*atomo)->info.atomo);
        free(*atomo);
        *atomo = NULL;
    }
}

void lista_gen_apagar(LISTA_G **lista){
    if(lista != NULL && (*lista) != NULL){
        NO *atual = (*lista)->inicio;
        NO *prox = NULL;
        while(atual != NULL){
            prox = atual->prox;
            if(atual->tipo == TIPO_ATOMO){
                atomo_apagar(&atual);
            }
            else{
                lista_gen_apagar(&atual->info.sublista);
                free(atual);
                atual = NULL;
            }
            atual = prox;
        }
        free(*lista);
        *lista = NULL;
    }
}

int lista_gen_get_profundidade(LISTA_G *lista){
    if(lista != NULL){
        int prof = 0, t;
        NO *atual = lista->inicio;
        if(atual == NULL){
            return 0;
        }
        while(atual != NULL){
            if(atual->tipo == TIPO_SUBLISTA){
                t = lista_gen_get_profundidade(atual->info.sublista);
                if(t > prof){
                    prof = t;
                }
            }
            atual = atual->prox;
        }
        return prof + 1;
    }
    return -1;
}