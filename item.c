#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "item.h"

//TAD item generico;

/* Despachante: nao conhece nenhuma coluna. Cada pergunta sobre um campo
 * eh repassada as ITEM_OPS do dataset dono do registro. */
struct item_
{
    const ITEM_OPS *ops;  //funcoes do dataset deste registro;
    void *dados;          //struct do dataset (asteroide, movimento...); o ITEM e dono;
    char *linha_original; //linha do CSV, gravada sem alteracao na saida; NULL ate ser definida;
};

//dataset da execucao: usado por item_campo_tipo, que nao recebe ITEM
//(a query valida os campos antes de qualquer registro existir);
static const ITEM_OPS *dataset_atual = NULL;

void item_definir_dataset(const ITEM_OPS *ops){
    dataset_atual = ops;
}

const ITEM_OPS *item_dataset(void){
    return dataset_atual;
}

ITEM *item_criar(const ITEM_OPS *ops, void *dados){
    if(ops == NULL || dados == NULL){
        return NULL;
    }
    ITEM *item = (ITEM*)malloc(sizeof(ITEM));
    if(item == NULL){
        return NULL; //quem chamou continua dono de "dados" e deve libera-los;
    }
    item->ops = ops;
    item->dados = dados;
    item->linha_original = NULL;
    return item;
}

void item_apagar(ITEM **item){
    if(item == NULL || *item == NULL){
        return;
    }
    if((*item)->ops->apagar_dados != NULL){
        (*item)->ops->apagar_dados((*item)->dados);
    }
    free((*item)->linha_original);
    free(*item);
    *item = NULL;
}

const void *item_get_dados(const ITEM *item){
    if(item == NULL) return NULL;
    return item->dados;
}

const ITEM_OPS *item_get_ops(const ITEM *item){
    if(item == NULL) return NULL;
    return item->ops;
}

bool item_set_linha_original(ITEM *item, const char *linha){
//linha original do csv eh guardada para possível impressao em arquivo de resposta;
    if(item == NULL || linha == NULL){
        return false;
    }
    size_t tamanho = strlen(linha) + 1;
    char *copia = (char*)malloc(tamanho);//copia string dinamicamente;
    if(copia == NULL){
        return false;
    }
    memcpy(copia, linha, tamanho);

    free(item->linha_original);
    item->linha_original = copia;
    return true;
}

const char *item_get_linha_original(const ITEM *item){
    if(item == NULL) return NULL;
    return item->linha_original;
}

/* --- ACESSO A CAMPOS POR NOME --- */

TIPO_CAMPO item_campo_tipo(const char *campo){
    if(dataset_atual == NULL || campo == NULL) return CAMPO_DESCONHECIDO;
    return dataset_atual->campo_tipo(campo);
}

//tipo do campo segundo o dataset do proprio registro;
static TIPO_CAMPO tipo_no_item(const ITEM *item, const char *campo){
    if(item == NULL || campo == NULL) return CAMPO_DESCONHECIDO;
    return item->ops->campo_tipo(campo);
}

const char *item_get_campo_texto(const ITEM *item, const char *campo){
    if(tipo_no_item(item, campo) != CAMPO_TEXTO) return NULL;
    return item->ops->get_texto(item->dados, campo);
}

double item_get_campo_num(const ITEM *item, const char *campo){
    if(tipo_no_item(item, campo) != CAMPO_NUMERO) return 0.0;
    return item->ops->get_num(item->dados, campo);
}

bool item_get_campo_bool(const ITEM *item, const char *campo){
    if(tipo_no_item(item, campo) != CAMPO_BOOL) return false;
    return item->ops->get_bool(item->dados, campo);
}

bool item_campo_ausente(const ITEM *item, const char *campo){
    TIPO_CAMPO tipo = tipo_no_item(item, campo);
    if(tipo == CAMPO_DESCONHECIDO) return false;
    if(item->ops->ausente != NULL && item->ops->ausente(item->dados, campo)) return true;
    if(tipo == CAMPO_TEXTO) return item->ops->get_texto(item->dados, campo) == NULL;
    if(tipo == CAMPO_NUMERO) return isnan(item->ops->get_num(item->dados, campo));
    return false;
}

void item_get_campo_str(const ITEM *item, const char *campo, char *dest, size_t tam_dest){
    if(dest == NULL || tam_dest == 0) return;

    TIPO_CAMPO tipo = tipo_no_item(item, campo);
    if(tipo == CAMPO_DESCONHECIDO){
        snprintf(dest, tam_dest, "DESCONHECIDO");
        return;
    }
    //o dataset pode ter formato proprio (ex.: NASA com casas decimais fixas);
    if(item->ops->formatar != NULL){
        item->ops->formatar(item->dados, campo, dest, tam_dest);
        return;
    }

    //formato padrao, igual para qualquer dataset;
    switch(tipo){
        case CAMPO_TEXTO: {
            const char *val = item->ops->get_texto(item->dados, campo);
            snprintf(dest, tam_dest, "%s", val ? val : "N/A");
            break;
        }
        case CAMPO_BOOL:
            snprintf(dest, tam_dest, "%s", item->ops->get_bool(item->dados, campo) ? "true" : "false");
            break;
        case CAMPO_NUMERO: {
            double val = item->ops->get_num(item->dados, campo);
            if(isnan(val)) snprintf(dest, tam_dest, "N/A");
            else snprintf(dest, tam_dest, "%.15g", val); //inteiro sai sem casas: 529215, nao 529215.000000;
            break;
        }
        default:
            snprintf(dest, tam_dest, "DESCONHECIDO");
            break;
    }
}
