#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include "leitor_sti.h"
#include "item_sti.h"
#include "csv.h"

/* Formato do CSV da STI: o que este leitor passa para csv.c. */
#define DELIMITADOR ';'
#define NUM_COLUNAS 5

struct leitor_sti_ { FILE *arquivo; char *cabecalho; };
static const char *colunas[NUM_COLUNAS] = {
    "UF_ATENDIMENTO", "TIPO", "CLASSIFICACAO", "NACIONALIDADE", "TOTAL"
};

static char *copiar(const char *s) {
    char *c = malloc(strlen(s) + 1);
    if (c) strcpy(c, s);
    return c;
}

LEITOR_STI *leitor_sti_abrir(const char *caminho) {
    FILE *arq = fopen(caminho, "rb");
    if (!arq) return NULL;
    char *cabecalho = NULL;
    if (csv_ler_registro(arq, &cabecalho) != 1) { fclose(arq); return NULL; }
    /* O cabecalho precisa ter exatamente as 5 colunas, nesta ordem. */
    char *copia = copiar(cabecalho), *campos[NUM_COLUNAS];
    bool valido = copia && csv_separar(copia, DELIMITADOR, campos, NUM_COLUNAS);
    if (valido) {
        for (size_t i = 0; i < NUM_COLUNAS; i++) {
            if (strcmp(campos[i], colunas[i])) { valido = false; break; }
        }
    }
    free(copia);
    if (!valido) { free(cabecalho); fclose(arq); return NULL; }
    LEITOR_STI *l = malloc(sizeof(*l));
    if (!l) { free(cabecalho); fclose(arq); return NULL; }
    l->arquivo = arq; l->cabecalho = cabecalho;
    return l;
}

/* TOTAL: numero finito e nao negativo; vazio vira NaN (ausente). */
static bool total_valido(const char *s, double *n) {
    if (!*s) { *n = NAN; return true; }
    char *fim;
    errno = 0; *n = strtod(s, &fim);
    return !errno && *fim == '\0' && isfinite(*n) && *n >= 0.0;
}

int leitor_sti_proximo(LEITOR_STI *l, ITEM **item) {
    if (!l || !item) return -1;
    *item = NULL;
    char *linha;
    int estado = csv_ler_registro(l->arquivo, &linha);
    if (estado != 1) return estado;
    char *copia = copiar(linha), *v[NUM_COLUNAS];
    ITEM *novo = NULL;
    double total;
    if (!copia || !csv_separar(copia, DELIMITADOR, v, NUM_COLUNAS)) goto erro;
    if (!total_valido(v[4], &total)) goto erro;
    /* NACIONALIDADE pode vir vazia (ausente): e aceita como "". */
    novo = item_sti_criar(v[0], v[1], v[2], v[3], total);
    if (!novo || !item_set_linha_original(novo, linha)) goto erro;
    free(copia); free(linha); *item = novo;
    return 1;
erro:
    item_apagar(&novo); free(copia); free(linha);
    return -1;
}

const char *leitor_sti_cabecalho(const LEITOR_STI *l) {
    return l ? l->cabecalho : NULL;
}

void leitor_sti_fechar(LEITOR_STI **l) {
    if (!l || !*l) return;
    fclose((*l)->arquivo); free((*l)->cabecalho); free(*l); *l = NULL;
}

/* Adaptadores para a interface comum (leitor.h), que usa void * no lugar de
 * LEITOR_STI *: cada um so converte o ponteiro e chama a funcao tipada. */
static void *abrir_generico(const char *caminho) { return leitor_sti_abrir(caminho); }
static int proximo_generico(void *l, ITEM **item) { return leitor_sti_proximo(l, item); }
static const char *cabecalho_generico(const void *l) { return leitor_sti_cabecalho(l); }
static void fechar_generico(void *l) { LEITOR_STI *p = l; leitor_sti_fechar(&p); }

const LEITOR_TIPO LEITOR_STI_TIPO = {
    "sti", &ITEM_OPS_STI,
    abrir_generico, proximo_generico, cabecalho_generico, fechar_generico
};
