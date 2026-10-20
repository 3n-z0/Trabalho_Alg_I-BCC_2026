#include "item.h"
#include "lista_cruz.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct celula_
{
    struct celula_ *direita, *abaixo;
    ITEM *item;
    int linha, coluna;
}CELULA;


struct lista_cruz_
{
    CELULA **linhas, **colunas;
    int nlinhas, ncolunas;
};

LISTA_CRUZ *lista_cruz_criar(int nlinhas, int ncolunas){
    if(nlinhas > 0 && ncolunas > 0){
        LISTA_CRUZ *lista_cruz = (LISTA_CRUZ*)malloc(sizeof(LISTA_CRUZ));
        if(lista_cruz != NULL){
            CELULA **linhas = (CELULA**)calloc(nlinhas, sizeof(CELULA*));
            CELULA **colunas = (CELULA**)calloc(ncolunas, sizeof(CELULA*));
            if(linhas != NULL && colunas != NULL){
                lista_cruz->linhas = linhas;
                lista_cruz->colunas = colunas;
                lista_cruz->nlinhas = nlinhas;
                lista_cruz->ncolunas = ncolunas;
                return lista_cruz;
            }
            free(linhas);
            free(colunas);
            free(lista_cruz);
        }
    }
    return NULL;
}

static void celula_apagar(CELULA **celula){
    if(celula != NULL && *celula != NULL){
        item_apagar(&(*celula)->item);
        free(*celula);
        *celula = NULL;
    }
}

static void lista_cruz_inserir_linha(CELULA **linha, CELULA *celula){
    if(linha[celula->linha] == NULL){
        linha[celula->linha] = celula;
        return;
    }
    if(linha[celula->linha]->coluna > celula->coluna){
        celula->direita = linha[celula->linha];
        linha[celula->linha] = celula;
        return;
    }
    CELULA *atual = linha[celula->linha];
    while(atual != NULL){
        if(atual->direita == NULL){
            atual->direita = celula;
            break;
        }
        if(celula->coluna < atual->direita->coluna){
            celula->direita = atual->direita;
            atual->direita = celula;
            break;
        }
        
        atual = atual->direita;
    }
}

static void lista_cruz_inserir_coluna(CELULA **coluna, CELULA *celula){
    if(coluna[celula->coluna] == NULL){
        coluna[celula->coluna] = celula;
        return;
    }
    if(coluna[celula->coluna]->linha > celula->linha){
        celula->abaixo = coluna[celula->coluna];
        coluna[celula->coluna] = celula;
        return;
    }
    CELULA *atual = coluna[celula->coluna];
    while(atual != NULL){
        if(atual->abaixo == NULL){
            atual->abaixo = celula;
            break;
        }
        if(celula->linha < atual->abaixo->linha){
            celula->abaixo = atual->abaixo;
            atual->abaixo = celula;
            break;
        }
        
        atual = atual->abaixo;
    }
}

bool lista_cruz_inserir(LISTA_CRUZ *matriz, ITEM *item, int linha, int coluna){
    if(matriz != NULL && item != NULL && linha >= 0 && coluna >= 0 && linha < matriz->nlinhas && coluna < matriz->ncolunas){
        CELULA *celula = (CELULA*)malloc(sizeof(CELULA));
        if(celula != NULL){
            celula->item = item;
            celula->linha = linha;
            celula->coluna = coluna;
            celula->abaixo = NULL;
            celula->direita = NULL;
            lista_cruz_inserir_linha(matriz->linhas, celula);
            lista_cruz_inserir_coluna(matriz->colunas, celula);
            return true;
        }
    }
    return false;
}

ITEM* lista_cruz_remover(LISTA_CRUZ *matriz, int linha, int coluna){
    if(matriz != NULL && linha >= 0 && coluna >= 0 && linha < matriz->nlinhas && coluna < matriz->ncolunas){
        CELULA *apaga = NULL;
        CELULA *anterior = NULL;
        apaga = matriz->linhas[linha];
        if(apaga == NULL){
            return NULL;
        }
        if(matriz->linhas[linha]->coluna == coluna){
            matriz->linhas[linha] = matriz->linhas[linha]->direita;
        }
        else{
            while(apaga != NULL){
                if(apaga->coluna == coluna){
                    anterior->direita = apaga->direita;
                    break;
                }
                else if(apaga->coluna > coluna){
                    break;
                }
            anterior = apaga;
            apaga = apaga->direita;
            }
        }
        apaga = matriz->colunas[coluna];
        anterior = NULL;
        if(apaga == NULL){
            return NULL;
        }
        if(matriz->colunas[coluna]->linha == linha){
            matriz->colunas[coluna] = matriz->colunas[coluna]->abaixo;
        }
        else{
            while(apaga != NULL){
                if(apaga->linha == linha){
                    anterior->abaixo = apaga->abaixo;
                    break;
                }
                else if(apaga->linha > linha){
                    apaga = NULL;
                    break;
                }
            anterior = apaga;
            apaga = apaga->abaixo;
            }
        }
        if(apaga != NULL){
            ITEM *item = apaga->item;
            free(apaga);
            return item;
        }
    }
    return NULL;
}

void lista_cruz_apagar(LISTA_CRUZ **matriz){
    if(matriz != NULL && (*matriz) != NULL){
        CELULA *atual = NULL;
        CELULA *apagar = NULL;
        for(int i = 0; i < (*matriz)->nlinhas; i++){
            atual = (*matriz)->linhas[i];
            while(atual != NULL){
                apagar = atual;
                atual = atual->direita;
                celula_apagar(&apagar);
            }
        }
        free((*matriz)->linhas);
        free((*matriz)->colunas);

        free(*matriz);
        *matriz = NULL;
    }
}