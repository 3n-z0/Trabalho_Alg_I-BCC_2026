#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "item.h"
#include "item_sti.h"

/* Dados de um movimento migratorio (uma linha do CSV da STI). Fica dentro
 * de um ITEM generico (item.c), que guarda tambem a linha original. */
typedef struct
{
    //textos alocados no tamanho exato do valor lido;
    char *uf_atendimento;
    char *tipo;           //ENTRADA ou SAIDA;
    char *classificacao;  //PERMANENTE, TEMPORARIO, VISITANTE ou OUTROS;
    char *nacionalidade;  //"" quando ausente na base;
    double total;         //quantidade de pessoas no movimento;
}ITEM_STI;

//copia do texto no tamanho exato; NULL vira "";
static char *copiar_texto(const char *s){
    if(s == NULL) s = "";
    size_t tamanho = strlen(s) + 1;
    char *copia = (char*)malloc(tamanho);
    if(copia != NULL) memcpy(copia, s, tamanho);
    return copia;
}

//libera os dados do movimento; e a funcao apagar_dados de ITEM_OPS_STI;
static void sti_apagar(void *dados){
    ITEM_STI *m = (ITEM_STI*) dados;
    if(m == NULL){
        return;
    }
    free(m->uf_atendimento);
    free(m->tipo);
    free(m->classificacao);
    free(m->nacionalidade);
    free(m);
}

ITEM *item_sti_criar(
const char *uf_atendimento,
const char *tipo,
const char *classificacao,
const char *nacionalidade,
double total){
    //calloc: ponteiros comecam NULL, entao sti_apagar libera com seguranca
    //um registro criado pela metade, se alguma alocacao abaixo falhar;
    ITEM_STI *m = (ITEM_STI*)calloc(1, sizeof(ITEM_STI));
    if(m == NULL){
        return NULL;
    }

    m->uf_atendimento = copiar_texto(uf_atendimento);
    m->tipo = copiar_texto(tipo);
    m->classificacao = copiar_texto(classificacao);
    m->nacionalidade = copiar_texto(nacionalidade);
    m->total = total;

    if(m->uf_atendimento == NULL || m->tipo == NULL ||
       m->classificacao == NULL || m->nacionalidade == NULL){
        sti_apagar(m);
        return NULL;
    }

    //o ITEM generico passa a ser dono de "m"; se falhar, ainda somos nos;
    ITEM *item = item_criar(&ITEM_OPS_STI, m);
    if(item == NULL){
        sti_apagar(m);
    }
    return item;
}

//dados do movimento dentro do ITEM; NULL se o ITEM nao for da STI;
static const ITEM_STI *sti(const ITEM *item){
    if(item == NULL || item_get_ops(item) != &ITEM_OPS_STI) return NULL;
    return (const ITEM_STI*) item_get_dados(item);
}

/* --- GETTERS ESPECIFICOS (recebem o ITEM generico) --- */

const char *item_sti_get_uf_atendimento(const ITEM *item){
    const ITEM_STI *m = sti(item);
    return m ? m->uf_atendimento : NULL;
}

const char *item_sti_get_tipo(const ITEM *item){
    const ITEM_STI *m = sti(item);
    return m ? m->tipo : NULL;
}

const char *item_sti_get_classificacao(const ITEM *item){
    const ITEM_STI *m = sti(item);
    return m ? m->classificacao : NULL;
}

const char *item_sti_get_nacionalidade(const ITEM *item){
    const ITEM_STI *m = sti(item);
    return m ? m->nacionalidade : NULL;
}

double item_sti_get_total(const ITEM *item){
    const ITEM_STI *m = sti(item);
    return m ? m->total : 0.0;
}

/* --- ACESSO A CAMPOS POR NOME ---
 * Os nomes sao exatamente os do cabecalho do CSV (em maiusculas), que sao
 * os usados nas consultas: FILTER|TOTAL|>=|1000, GROUP|UF_ATENDIMENTO|TIPO... */
typedef enum {
    CP_UF_ATENDIMENTO,
    CP_TIPO,
    CP_CLASSIFICACAO,
    CP_NACIONALIDADE,
    CP_TOTAL,
    CP_QTD
} ID_CAMPO;

typedef struct {
    const char *nome;
    TIPO_CAMPO tipo;
} DESCRITOR_CAMPO;

static const DESCRITOR_CAMPO CAMPOS[CP_QTD] = {
    [CP_UF_ATENDIMENTO] = {"UF_ATENDIMENTO", CAMPO_TEXTO},
    [CP_TIPO]           = {"TIPO",           CAMPO_TEXTO},
    [CP_CLASSIFICACAO]  = {"CLASSIFICACAO",  CAMPO_TEXTO},
    [CP_NACIONALIDADE]  = {"NACIONALIDADE",  CAMPO_TEXTO},
    [CP_TOTAL]          = {"TOTAL",          CAMPO_NUMERO}
};

//indice do campo em CAMPOS, ou -1 se o nome nao existir;
static int campo_id(const char *campo){
    if (campo == NULL) return -1;

    for (int i = 0; i < CP_QTD; i++) {
        if (strcmp(campo, CAMPOS[i].nome) == 0) return i;
    }
    return -1;
}

/* --- OPERACOES DO DATASET (chamadas por item.c) ---
 * item.c ja confere o tipo do campo antes de chamar get_texto/get_num. */

static TIPO_CAMPO sti_campo_tipo(const char *campo){
    int id = campo_id(campo);
    return id < 0 ? CAMPO_DESCONHECIDO : CAMPOS[id].tipo;
}

static const char *sti_get_texto(const void *dados, const char *campo){
    const ITEM_STI *m = (const ITEM_STI*) dados;
    switch (campo_id(campo)) {
        case CP_UF_ATENDIMENTO: return m->uf_atendimento;
        case CP_TIPO:           return m->tipo;
        case CP_CLASSIFICACAO:  return m->classificacao;
        //nacionalidade vazia e ausente: NULL vira "N/A" nas chaves de GROUP/CROSS;
        case CP_NACIONALIDADE:  return m->nacionalidade[0] != '\0' ? m->nacionalidade : NULL;
        default:                return NULL;
    }
}

static double sti_get_num(const void *dados, const char *campo){
    const ITEM_STI *m = (const ITEM_STI*) dados;
    return campo_id(campo) == CP_TOTAL ? m->total : 0.0;
}

//a STI nao tem campos booleanos;
static bool sti_get_bool(const void *dados, const char *campo){
    (void) dados;
    (void) campo;
    return false;
}

/* formatar = NULL: o formato padrao de item.c ja serve (texto como esta,
 * "N/A" se ausente e TOTAL sem casas decimais, ex.: 529215). */
const ITEM_OPS ITEM_OPS_STI = {
    "sti",
    sti_campo_tipo,
    sti_get_texto,
    sti_get_num,
    sti_get_bool,
    NULL,
    sti_apagar,
    NULL //ausente: nacionalidade vazia ja vira NULL em sti_get_texto;
};
