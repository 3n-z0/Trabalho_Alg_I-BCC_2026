#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <limits.h>
#include "leitor_nasa.h"

struct leitor_nasa_ { FILE *arquivo; char *cabecalho; };
static const char *colunas[] = {
    "asteroid_designation", "asteroid_fullname", "close_approach_date", "year", "month",
    "distance_au", "velocity_km_s", "absolute_magnitude", "days_until_approach",
    "is_past_event", "is_future_event", "risk_score", "panic_level", "threat_category",
    "panic_verdict", "on_sentry_list", "sentry_impact_prob", "sentry_torino_scale",
    "sentry_palermo_scale", "sentry_diameter_km"
};

/* Le um registro CSV completo; aspas permitem virgulas e quebras internas. */
static int ler_registro(FILE *arquivo, char **linha) {
    *linha = NULL;
    size_t capacidade = 256, n = 0;
    char *buf = malloc(capacidade);
    if (!buf) return -1;
    bool aspas = false;
    int c;
    while ((c = fgetc(arquivo)) != EOF) {
        if (n + 1 >= capacidade) {
            if (capacidade > (size_t)-1 / 2) { free(buf); return -1; }
            capacidade *= 2;
            char *novo = realloc(buf, capacidade);
            if (!novo) { free(buf); return -1; }
            buf = novo;
        }
        if (c == 0) { free(buf); return -1; }
        buf[n++] = (char)c;
        if (c == '"') aspas = !aspas;
        if (c == '\n' && !aspas) break;
    }
    if (ferror(arquivo) || aspas) { free(buf); return -1; }
    if (!n) { free(buf); return 0; }
    buf[n] = '\0'; *linha = buf;
    return 1;
}

/* Separa no proprio buffer, preservando campos vazios e decodificando "". */
static bool separar(char *linha, char *campos[20]) {
    char *r = linha, *w = linha;
    size_t n = 0;
    for (;;) {
        if (n == 20) return false;
        campos[n++] = w;
        if (*r == '"') {
            r++;
            bool fechou = false;
            while (*r) {
                if (*r == '"') {
                    if (r[1] == '"') { *w++ = '"'; r += 2; }
                    else { r++; fechou = true; break; }
                } else *w++ = *r++;
            }
            if (!fechou) return false;
        } else {
            while (*r && *r != ',' && *r != '\r' && *r != '\n') {
                if (*r == '"') return false;
                *w++ = *r++;
            }
        }
        char sep = *r;
        if (sep != ',' && sep != '\0' && sep != '\r' && sep != '\n') return false;
        if (sep == ',') r++;
        else {
            if (*r == '\r') r++;
            if (*r == '\n') r++;
            if (*r) return false;
        }
        *w++ = '\0';
        if (sep != ',') return n == 20;
    }
}

static char *copiar(const char *s) {
    char *c = malloc(strlen(s) + 1);
    if (c) strcpy(c, s);
    return c;
}

LEITOR_NASA *leitor_nasa_abrir(const char *caminho) {
    FILE *arq = fopen(caminho, "rb");
    if (!arq) return NULL;
    char *cabecalho = NULL;
    if (ler_registro(arq, &cabecalho) != 1) { fclose(arq); return NULL; }
    char *copia = copiar(cabecalho), *campos[20];
    bool valido = copia && separar(copia, campos);
    if (valido) {
        for (size_t i = 0; i < 20; i++) {
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
    int estado = ler_registro(l->arquivo, &linha);
    if (estado != 1) return estado;
    char *copia = copiar(linha), *v[20];
    ITEM *novo = NULL;
    if (!copia || !separar(copia, v)) goto erro;
    /* Sem limite de tamanho nos textos: item_criar copia cada um no tamanho exato. */
    long long ano, mes, dias, panico;
    bool passado, futuro, sentry;
    double distancia, velocidade, magnitude, risco, prob, torino, palermo, diametro;
    if (!inteiro(v[3], 0, UINT_MAX, &ano) || !inteiro(v[4], 0, UINT_MAX, &mes) ||
        !inteiro(v[8], INT_MIN, INT_MAX, &dias) || !inteiro(v[12], 0, UINT_MAX, &panico) ||
        !booleano(v[9], &passado) || !booleano(v[10], &futuro) || !booleano(v[15], &sentry) ||
        !numero(v[5], &distancia) || !numero(v[6], &velocidade) || !numero(v[7], &magnitude) ||
        !numero(v[11], &risco) || !numero(v[16], &prob) || !numero(v[17], &torino) ||
        !numero(v[18], &palermo) || !numero(v[19], &diametro)) goto erro;
    novo = item_criar(v[0], v[1], v[2], (unsigned int)ano, (unsigned int)mes,
        distancia, velocidade, (float)magnitude, (int)dias, passado, futuro, risco,
        (unsigned int)panico, v[13], v[14], sentry, v[16], (float)torino, (float)palermo, (float)diametro);
    if (!novo || !item_set_linha_original(novo, linha)) goto erro;
    free(copia); free(linha); *item = novo;
    return 1;
erro:
    item_apagar(&novo); free(copia); free(linha);
    return -1;
}

void leitor_nasa_fechar(LEITOR_NASA **l) {
    if (!l || !*l) return;
    fclose((*l)->arquivo); free((*l)->cabecalho); free(*l); *l = NULL;
}

typedef struct { FILE *arquivo; size_t gravados; bool ok; const char *quebra; } Escrita;

static bool escrever_linha(FILE *arquivo, const char *linha, const char *quebra) {
    if (!linha || fputs(linha, arquivo) == EOF) return false;
    size_t n = strlen(linha);
    /* O ultimo registro sem newline pode mudar de posicao na estrutura. */
    return n && linha[n-1] == '\n' ? true : fputs(quebra, arquivo) != EOF;
}

static void escrever_item(ITEM *item, void *ctx) {
    Escrita *e = ctx;
    if (!e->ok) return;
    e->ok = escrever_linha(e->arquivo, item_get_linha_original(item), e->quebra);
    if (e->ok) e->gravados++;
}

bool leitor_nasa_salvar(const LEITOR_NASA *l, const char *caminho,
                       const ESTRUTURA *resultado, size_t top_k, size_t *gravados) {
    if (!l || !caminho || !resultado || !gravados) return false;
    *gravados = 0;
    FILE *arq = fopen(caminho, "wb");
    if (!arq) return false;
    const char *quebra = strstr(l->cabecalho, "\r\n") ? "\r\n" : "\n";
    Escrita e = {arq, 0, escrever_linha(arq, l->cabecalho, quebra), quebra};
    if (e.ok) estrutura_percorrer(resultado, top_k, escrever_item, &e);
    if (fclose(arq) != 0) e.ok = false;
    *gravados = e.gravados;
    return e.ok;
}
