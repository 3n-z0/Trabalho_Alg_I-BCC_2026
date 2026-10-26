#include "item.h"
#include "lista_cruz.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct celula_
{
    struct celula_ *direita, *abaixo;
    ITEM *item;
}CELULA;

typedef struct cabecalho_ {
    char *chave; //copia alocada no tamanho exato do valor;
    CELULA *primeiro;
    CELULA *ultimo;
    struct cabecalho_ *prox;
} CABECALHO;

struct lista_cruz_
{
    CABECALHO *linhas, *colunas;
    int qtd_itens;
};


LISTA_CRUZ *lista_cruz_criar(void){
    LISTA_CRUZ *lc = (LISTA_CRUZ*)malloc(sizeof(LISTA_CRUZ));
    if(lc == NULL) return NULL;
    lc->linhas = NULL;
    lc->colunas = NULL;
    lc->qtd_itens = 0;
    return lc;
}

static CABECALHO *criar_ou_encontrar_cabecalho(CABECALHO **lista_cabecalhos, const char* chave){
    if(chave == NULL) return NULL;

    CABECALHO *atual = *lista_cabecalhos;
    CABECALHO *anterior = NULL;

    while(atual != NULL){
        if(strcmp(chave, atual->chave) == 0){
            return atual;
        }
        anterior = atual;
        atual = atual->prox;
    }

    CABECALHO *novo = (CABECALHO*)malloc(sizeof(CABECALHO));

    if(novo == NULL) return NULL;

    size_t tamanho = strlen(chave) + 1;
    novo->chave = (char*)malloc(tamanho);
    if(novo->chave == NULL){
        free(novo);
        return NULL;
    }
    memcpy(novo->chave, chave, tamanho);
    novo->primeiro = NULL;
    novo->ultimo = NULL;
    novo->prox = NULL;

    if(*lista_cabecalhos == NULL){
        *lista_cabecalhos = novo;
    }
    else{
        anterior->prox = novo;
    }
    return novo;
}

bool lista_cruz_inserir(LISTA_CRUZ *lc, ITEM *item, const char* lin, const char* col){
    if(lc == NULL || item == NULL || lin == NULL || col == NULL) return false;

    CABECALHO *cab_lin = criar_ou_encontrar_cabecalho(&lc->linhas,lin);
    CABECALHO *cab_col = criar_ou_encontrar_cabecalho(&lc->colunas,col);

    if(cab_lin == NULL || cab_col == NULL) return false;


    CELULA *novo = (CELULA*)malloc(sizeof(CELULA));
    if(novo == NULL) return false;

    novo->item = item;
    novo->direita = NULL;
    novo->abaixo = NULL;

    if(cab_lin->ultimo == NULL){
        cab_lin->primeiro = novo;
    }
    else{
        cab_lin->ultimo->direita = novo;
    }
    cab_lin->ultimo = novo;

    if(cab_col->ultimo == NULL){
        cab_col->primeiro = novo;
    }
    else{
        cab_col->ultimo->abaixo = novo;
    }
    cab_col->ultimo = novo;

    lc->qtd_itens++;

    return true;
}

void lista_cruz_apagar(LISTA_CRUZ **lc){
    if(lc == NULL || *lc == NULL) return;

    CABECALHO *lin = (*lc)->linhas;

    while(lin != NULL){
        CELULA *cel = lin->primeiro;
        while(cel != NULL){
            CELULA *apaga = cel;
            cel = cel->direita;

            free(apaga);
        }

        CABECALHO *apaga = lin;
        lin = lin->prox;
        free(apaga->chave);
        free(apaga);
    }

    //as celulas ja foram liberadas pelas linhas; aqui so os cabecalhos de coluna;
    CABECALHO *col = (*lc)->colunas;
    while(col){
        CABECALHO *apaga = col;
        col = col->prox;

        free(apaga->chave);
        free(apaga);
    }

    free(*lc);
    *lc = NULL;
}

size_t lista_cruz_percorrer_linhas_limite(const LISTA_CRUZ *lc, size_t limite,
                                          void (*visita)(ITEM *item, void *ctx), void *ctx){
    if(lc == NULL || visita == NULL) return 0;

    size_t visitados = 0;
    CABECALHO *lin = lc->linhas;

    while(lin != NULL){
        CELULA *atual = lin->primeiro;
        while(atual != NULL){
            if(limite != 0 && visitados >= limite) return visitados;
            visita(atual->item, ctx);
            visitados++;
            atual = atual->direita;
        }
        lin = lin->prox;
    }
    return visitados;
}

void lista_cruz_percorrer_linhas(const LISTA_CRUZ *lc, void (*visita)(ITEM *item, void *ctx), void *ctx){
    (void) lista_cruz_percorrer_linhas_limite(lc, 0, visita, ctx);
}

size_t lista_cruz_percorrer_colunas_limite(const LISTA_CRUZ *lc, size_t limite,
                                           void (*visita)(ITEM *item, void *ctx), void *ctx){
    if(lc == NULL || visita == NULL) return 0;

    size_t visitados = 0;
    CABECALHO *col = lc->colunas;

    while(col != NULL){
        CELULA *atual = col->primeiro;
        while(atual != NULL){
            if(limite != 0 && visitados >= limite) return visitados;
            visita(atual->item, ctx);
            visitados++;
            atual = atual->abaixo;
        }
        col = col->prox;
    }
    return visitados;
}

void lista_cruz_percorrer_colunas(const LISTA_CRUZ *lc, void (*visita)(ITEM *item, void *ctx), void *ctx){
    (void) lista_cruz_percorrer_colunas_limite(lc, 0, visita, ctx);
}
