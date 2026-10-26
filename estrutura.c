#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estrutura.h"
#include "pilha.h"
#include "fila.h"
#include "lista_seq.h"
#include "lista_enc.h"
#include "lista_cabeca.h"
#include "lista_ord.h"
#include "lista_gen.h"
#include "lista_cruz.h"

/* Como a lista_ord obtem a chave de cada ITEM; decidido uma vez em estrutura_criar. */
typedef enum { ORD_SIMILARIDADE, ORD_NUMERICO, ORD_TEXTO } ModoOrd;

/* Adaptador: escolhe o TAD, sem acessar sua representacao interna. */
struct estrutura_ {
    TipoResultado tipo; void *dados; QUERY *query;
    ModoOrd modo_ord;      /* so para RESULT_LISTA_ORD */
    const char *campo_ord; /* emprestado da QUERY */
};

/* A query devolve "" quando o comando nao aparece. */
static bool definido(const char *s) { return s && *s; }

/* Chave de agrupamento (GROUP/CROSS) ou de ordenacao textual. Campo de texto:
 * devolve o proprio texto do ITEM, sem copia e sem limite de tamanho ("N/A" se
 * ausente). Demais tipos: formata em buf, que comporta qualquer numero/bool. */
static const char *chave_do_campo(const ITEM *item, const char *campo, char *buf, size_t tam) {
    if (item_campo_tipo(campo) == CAMPO_TEXTO) {
        const char *texto = item_get_campo_texto(item, campo);
        return texto ? texto : "N/A";
    }
    item_get_campo_str(item, campo, buf, tam);
    return buf;
}

bool estrutura_tipo(const char *nome, TipoResultado *tipo) {
    static const char *nomes[] = {"pilha", "fila", "lista_seq", "lista_enc",
        "lista_cabeca", "lista_ord", "lista_gen", "lista_cruz"};
    if (!nome || !tipo) return false;
    for (size_t i = 0; i < sizeof(nomes)/sizeof(*nomes); i++) {
        if (!strcmp(nome, nomes[i])) { *tipo = (TipoResultado)i; return true; }
    }
    return false;
}

ESTRUTURA *estrutura_criar(TipoResultado tipo, QUERY *query) {
    if (!query) return NULL;
    /* As estruturas avancadas dependem de comandos da consulta: sem eles,
     * todos os itens cairiam numa chave vazia/"DESCONHECIDO". */
    if (tipo == RESULT_LISTA_ORD && !definido(query_get_order_campo(query))) {
        fprintf(stderr, "Erro: lista_ord exige ORDER|campo|ASC ou DESC na consulta.\n");
        return NULL;
    }
    if (tipo == RESULT_LISTA_GEN && !definido(query_get_group_campo1(query))) {
        fprintf(stderr, "Erro: lista_gen exige GROUP|campo1|campo2 na consulta.\n");
        return NULL;
    }
    if (tipo == RESULT_LISTA_CRUZ && !definido(query_get_cross_campo1(query))) {
        fprintf(stderr, "Erro: lista_cruz exige CROSS|campo1|campo2 na consulta.\n");
        return NULL;
    }
    ESTRUTURA *e = calloc(1, sizeof(*e));
    if (!e) return NULL;
    e->tipo = tipo; e->query = query;
    switch (tipo) {
        case RESULT_PILHA: e->dados = pilha_criar(); break;
        case RESULT_FILA: e->dados = fila_criar(); break;
        case RESULT_LISTA_SEQ: e->dados = lista_seq_criar(); break;
        case RESULT_LISTA_ENC: e->dados = lista_enc_criar(); break;
        case RESULT_LISTA_CABECA: e->dados = lista_cabeca_criar(); break;
        case RESULT_LISTA_GEN: e->dados = lista_gen_criar(); break;
        case RESULT_LISTA_CRUZ: e->dados = lista_cruz_criar(); break;
        case RESULT_LISTA_ORD: {
            /* Decide aqui, uma vez, como obter a chave; estrutura_inserir so consulta modo_ord. */
            const char *campo = query_get_order_campo(query);
            if (!strcmp(campo, "similarity")) e->modo_ord = ORD_SIMILARIDADE;
            else if (item_campo_tipo(campo) == CAMPO_NUMERO) e->modo_ord = ORD_NUMERICO;
            else e->modo_ord = ORD_TEXTO;
            e->campo_ord = campo;

            LISTA_ORD_ORDEM ordem = !strcmp(query_get_order_direcao(query), "ASC") ? LISTA_ORD_ASC : LISTA_ORD_DESC;
            e->dados = lista_ord_criar(e->modo_ord == ORD_TEXTO ? LISTA_ORD_TEXTUAL : LISTA_ORD_NUMERICA, ordem);
            break;
        }
        default: break;
    }
    if (!e->dados) { free(e); return NULL; }
    return e;
}

bool estrutura_inserir(ESTRUTURA *e, ITEM *item, double similaridade) {
    if (!e || !item) return false;
    switch (e->tipo) {
        case RESULT_PILHA: return pilha_empilhar(e->dados, item);
        case RESULT_FILA: return fila_enfileirar(e->dados, item);
        case RESULT_LISTA_SEQ: return lista_seq_inserir(e->dados, item);
        case RESULT_LISTA_ENC: return lista_enc_inserir(e->dados, item);
        case RESULT_LISTA_CABECA: return lista_cabeca_inserir_fim(e->dados, item);
        case RESULT_LISTA_GEN:
        case RESULT_LISTA_CRUZ: {
            bool grupo = e->tipo == RESULT_LISTA_GEN;
            const char *c1 = grupo ? query_get_group_campo1(e->query) : query_get_cross_campo1(e->query);
            const char *c2 = grupo ? query_get_group_campo2(e->query) : query_get_cross_campo2(e->query);
            char b1[64], b2[64];
            const char *v1 = chave_do_campo(item, c1, b1, sizeof(b1));
            const char *v2 = chave_do_campo(item, c2, b2, sizeof(b2));
            return grupo ? lista_gen_inserir_agrupado(e->dados, item, v1, v2) :
                           lista_cruz_inserir(e->dados, item, v1, v2);
        }
        case RESULT_LISTA_ORD:
            switch (e->modo_ord) {
                case ORD_SIMILARIDADE:
                    return lista_ord_inserir_numero(e->dados, item, similaridade);
                case ORD_NUMERICO:
                    return lista_ord_inserir_numero(e->dados, item, item_get_campo_num(item, e->campo_ord));
                case ORD_TEXTO: {
                    char buf[64];
                    return lista_ord_inserir_texto(e->dados, item, chave_do_campo(item, e->campo_ord, buf, sizeof(buf)));
                }
            }
            return false;
        default: return false;
    }
}

size_t estrutura_percorrer(const ESTRUTURA *e, size_t limite,
                          void (*visita)(ITEM *, void *), void *ctx) {
    if (!e || !visita) return 0;
    switch (e->tipo) {
        case RESULT_PILHA: return pilha_percorrer_limite(e->dados, limite, visita, ctx);
        case RESULT_FILA: return fila_percorrer_limite(e->dados, limite, visita, ctx);
        case RESULT_LISTA_SEQ: return lista_seq_percorrer_limite(e->dados, limite, visita, ctx);
        case RESULT_LISTA_ENC: return lista_enc_percorrer_limite(e->dados, limite, visita, ctx);
        case RESULT_LISTA_CABECA: return lista_cabeca_percorrer_limite(e->dados, limite, visita, ctx);
        case RESULT_LISTA_ORD: return lista_ord_percorrer_limite(e->dados, limite, visita, ctx);
        case RESULT_LISTA_GEN: return lista_gen_percorrer_limite(e->dados, limite, visita, ctx);
        case RESULT_LISTA_CRUZ: return lista_cruz_percorrer_linhas_limite(e->dados, limite, visita, ctx);
        default: return 0;
    }
}

static void destruir_item(ITEM *item, void *ctx) {
    (void)ctx;
    item_apagar(&item);
}

void estrutura_apagar(ESTRUTURA **ptr, bool liberar_itens) {
    if (!ptr || !*ptr) return;
    ESTRUTURA *e = *ptr;
    if (liberar_itens) estrutura_percorrer(e, 0, destruir_item, NULL);
    /* Variaveis do tipo correto evitam converter void** em ponteiro para TAD*. */
    switch (e->tipo) {
        case RESULT_PILHA: { PILHA *p = e->dados; pilha_apagar(&p); break; }
        case RESULT_FILA: { FILA *p = e->dados; fila_apagar(&p); break; }
        case RESULT_LISTA_SEQ: { LISTA_SEQ *p = e->dados; lista_seq_apagar(&p); break; }
        case RESULT_LISTA_ENC: { LISTA_ENC *p = e->dados; lista_enc_apagar(&p); break; }
        case RESULT_LISTA_CABECA: { LISTA_CABECA *p = e->dados; lista_cabeca_apagar(&p); break; }
        case RESULT_LISTA_ORD: { LISTA_ORD *p = e->dados; lista_ord_apagar(&p); break; }
        case RESULT_LISTA_GEN: { LISTA_GEN *p = e->dados; lista_gen_apagar(&p); break; }
        case RESULT_LISTA_CRUZ: { LISTA_CRUZ *p = e->dados; lista_cruz_apagar(&p); break; }
    }
    free(e); *ptr = NULL;
}
