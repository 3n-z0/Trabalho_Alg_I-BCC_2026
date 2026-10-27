#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <limits.h>
#include "leitor_nasa.h"
#include "item_nasa.h"
#include "csv.h"

/* Formato do CSV de asteroides: o que este leitor passa para csv.c. */
#define DELIMITADOR ','
#define NUM_COLUNAS 20

struct leitor_nasa_ { FILE *arquivo; char *cabecalho; };
static const char *colunas[NUM_COLUNAS] = {
    "asteroid_designation", "asteroid_fullname", "close_approach_date", "year", "month",
    "distance_au", "velocity_km_s", "absolute_magnitude", "days_until_approach",
    "is_past_event", "is_future_event", "risk_score", "panic_level", "threat_category",
    "panic_verdict", "on_sentry_list", "sentry_impact_prob", "sentry_torino_scale",
    "sentry_palermo_scale", "sentry_diameter_km"
};

static char *copiar(const char *s) {
    char *c = malloc(strlen(s) + 1);
    if (c) strcpy(c, s);
    return c;
}

LEITOR_NASA *leitor_nasa_abrir(const char *caminho) {
    FILE *arq = fopen(caminho, "rb");
    if (!arq) return NULL;
    char *cabecalho = NULL;
    if (csv_ler_registro(arq, &cabecalho) != 1) { fclose(arq); return NULL; }
    char *copia = copiar(cabecalho), *campos[NUM_COLUNAS];
    bool valido = copia && csv_separar(copia, DELIMITADOR, campos, NUM_COLUNAS);
    if (valido) {
        for (size_t i = 0; i < NUM_COLUNAS; i++) {
            if (strcmp(campos[i], colunas[i])) { valido = false; break; }
        }
    }
    free(copia);
    if (!valido) { free(cabecalho); fclose(arq); return NULL; }
    LEITOR_NASA *l = malloc(sizeof(*l));
    if (!l) { free(cabecalho); fclose(arq); return NULL; }
    l->arquivo = arq; l->cabecalho = cabecalho;
    return l;
}

static bool numero(const char *s, double *n) {
    if (!*s) { *n = NAN; return true; }
    char *fim;
    errno = 0; *n = strtod(s, &fim);
    return !errno && *fim == '\0' && isfinite(*n);
}

static bool inteiro(const char *s, long long minimo, long long maximo, long long *n) {
    if (!*s) { *n = 0; return true; }
    char *fim;
    errno = 0; *n = strtoll(s, &fim, 10);
    return !errno && *fim == '\0' && *n >= minimo && *n <= maximo;
}

static bool booleano(const char *s, bool *b) {
    if (!strcmp(s, "True") || !strcmp(s, "true") || !strcmp(s, "1")) { *b = true; return true; }
    if (!*s || !strcmp(s, "False") || !strcmp(s, "false") || !strcmp(s, "0")) { *b = false; return true; }
    return false;
}

int leitor_nasa_proximo(LEITOR_NASA *l, ITEM **item) {
    if (!l || !item) return -1;
    *item = NULL;
    char *linha;
    int estado = csv_ler_registro(l->arquivo, &linha);
    if (estado != 1) return estado;
    char *copia = copiar(linha), *v[NUM_COLUNAS];
    ITEM *novo = NULL;
    if (!copia || !csv_separar(copia, DELIMITADOR, v, NUM_COLUNAS)) goto erro;
    /* Sem limite de tamanho nos textos: item_nasa_criar copia cada um no tamanho exato. */
    long long ano, mes, dias, panico;
    bool passado, futuro, sentry;
    double distancia, velocidade, magnitude, risco, prob, torino, palermo, diametro;
    if (!inteiro(v[3], 0, UINT_MAX, &ano) || !inteiro(v[4], 0, UINT_MAX, &mes) ||
        !inteiro(v[8], INT_MIN, INT_MAX, &dias) || !inteiro(v[12], 0, UINT_MAX, &panico) ||
        !booleano(v[9], &passado) || !booleano(v[10], &futuro) || !booleano(v[15], &sentry) ||
        !numero(v[5], &distancia) || !numero(v[6], &velocidade) || !numero(v[7], &magnitude) ||
        !numero(v[11], &risco) || !numero(v[16], &prob) || !numero(v[17], &torino) ||
        !numero(v[18], &palermo) || !numero(v[19], &diametro)) goto erro;
    novo = item_nasa_criar(v[0], v[1], v[2], (unsigned int)ano, (unsigned int)mes,
        distancia, velocidade, (float)magnitude, (int)dias, passado, futuro, risco,
        (unsigned int)panico, v[13], v[14], sentry, v[16], (float)torino, (float)palermo, (float)diametro);
    if (!novo || !item_set_linha_original(novo, linha)) goto erro;
    free(copia); free(linha); *item = novo;
    return 1;
erro:
    item_apagar(&novo); free(copia); free(linha);
    return -1;
}

const char *leitor_nasa_cabecalho(const LEITOR_NASA *l) {
    return l ? l->cabecalho : NULL;
}

void leitor_nasa_fechar(LEITOR_NASA **l) {
    if (!l || !*l) return;
    fclose((*l)->arquivo); free((*l)->cabecalho); free(*l); *l = NULL;
}

/* Adaptadores para a interface comum (leitor.h), que usa void * no lugar de
 * LEITOR_NASA *: cada um so converte o ponteiro e chama a funcao tipada. */
static void *abrir_generico(const char *caminho) { return leitor_nasa_abrir(caminho); }
static int proximo_generico(void *l, ITEM **item) { return leitor_nasa_proximo(l, item); }
static const char *cabecalho_generico(const void *l) { return leitor_nasa_cabecalho(l); }
static void fechar_generico(void *l) { LEITOR_NASA *p = l; leitor_nasa_fechar(&p); }

const LEITOR_TIPO LEITOR_NASA_TIPO = {
    "nasa", &ITEM_OPS_NASA,
    abrir_generico, proximo_generico, cabecalho_generico, fechar_generico
};
