#include <stdlib.h>
#include <string.h>
#include "lista_gen.h"

typedef struct no_ NO;

typedef enum {
    TIPO_ATOMO,
    TIPO_SUBLISTA
} TIPO_NO;

struct lista_gen_ {
    NO *inicio;
    NO *fim;
};

struct no_ {
    TIPO_NO tipo;
    union {
        ITEM *atomo;
        LISTA_GEN *sublista;
    } info;
    char *chave;
    NO *prox;
};

LISTA_GEN *lista_gen_criar(void) {
    LISTA_GEN *lista = malloc(sizeof(*lista));
    if (lista != NULL) {
        lista->inicio = NULL;
        lista->fim = NULL;
    }
    return lista;
}

static void anexar_no(LISTA_GEN *lista, NO *no) {
    no->prox = NULL;
    if (lista->fim == NULL) {
        lista->inicio = no;
    } else {
        lista->fim->prox = no;
    }
    lista->fim = no;
}

bool lista_gen_inserir_atomo(LISTA_GEN *lista, ITEM *item) {
    if (lista == NULL || item == NULL) return false;

    NO *no = malloc(sizeof(*no));
    if (no == NULL) return false;

    no->tipo = TIPO_ATOMO;
    no->info.atomo = item;
    no->chave = NULL;
    anexar_no(lista, no);
    return true;
}

static NO *criar_no_sublista(const char *chave) {
    NO *no = malloc(sizeof(*no));
    if (no == NULL) return NULL;

    no->tipo = TIPO_SUBLISTA;
    no->chave = NULL;
    no->prox = NULL;
    no->info.sublista = lista_gen_criar();
    if (no->info.sublista == NULL) {
        free(no);
        return NULL;
    }

    if (chave != NULL) {
        size_t tamanho = strlen(chave) + 1;
        no->chave = malloc(tamanho);
        if (no->chave == NULL) {
            lista_gen_apagar(&no->info.sublista);
            free(no);
            return NULL;
        }
        memcpy(no->chave, chave, tamanho);
    }
    return no;
}

static NO *encontrar_grupo(const LISTA_GEN *lista, const char *chave) {
    for (NO *atual = lista->inicio; atual != NULL; atual = atual->prox) {
        if (atual->tipo == TIPO_SUBLISTA && atual->chave != NULL &&
            strcmp(atual->chave, chave) == 0) {
            return atual;
        }
    }
    return NULL;
}

static void apagar_no(NO *no) {
    if (no == NULL) return;

    if (no->tipo == TIPO_SUBLISTA) {
        lista_gen_apagar(&no->info.sublista);
    }
    free(no->chave);
    free(no);
}

bool lista_gen_inserir_agrupado(LISTA_GEN *lista, ITEM *item,
                              const char *grupo, const char *subgrupo) {
    if (lista == NULL || item == NULL || grupo == NULL || subgrupo == NULL) {
        return false;
    }

    NO *no_grupo = encontrar_grupo(lista, grupo);
    NO *novo_grupo = NULL;
    if (no_grupo == NULL) {
        novo_grupo = criar_no_sublista(grupo);
        if (novo_grupo == NULL) return false;
        no_grupo = novo_grupo;
    }

    NO *no_subgrupo = encontrar_grupo(no_grupo->info.sublista, subgrupo);
    NO *novo_subgrupo = NULL;
    if (no_subgrupo == NULL) {
        novo_subgrupo = criar_no_sublista(subgrupo);
        if (novo_subgrupo == NULL) {
            apagar_no(novo_grupo);
            return false;
        }
        no_subgrupo = novo_subgrupo;
    }

    if (!lista_gen_inserir_atomo(no_subgrupo->info.sublista, item)) {
        apagar_no(novo_subgrupo);
        apagar_no(novo_grupo);
        return false;
    }

    if (novo_subgrupo != NULL) {
        anexar_no(no_grupo->info.sublista, novo_subgrupo);
    }
    if (novo_grupo != NULL) {
        anexar_no(lista, novo_grupo);
    }
    return true;
}

static void percorrer_recursivo(const LISTA_GEN *lista, size_t limite,
                               void (*visita)(ITEM *item, void *ctx),
                               void *ctx, size_t *visitados) {
    for (NO *atual = lista->inicio; atual != NULL; atual = atual->prox) {
        if (limite != 0 && *visitados >= limite) return;

        if (atual->tipo == TIPO_ATOMO) {
            visita(atual->info.atomo, ctx);
            (*visitados)++;
        } else {
            percorrer_recursivo(atual->info.sublista, limite,
                               visita, ctx, visitados);
        }
    }
}

size_t lista_gen_percorrer_limite(const LISTA_GEN *lista, size_t limite,
                                void (*visita)(ITEM *item, void *ctx), void *ctx) {
    if (lista == NULL || visita == NULL) return 0;

    size_t visitados = 0;
    percorrer_recursivo(lista, limite, visita, ctx, &visitados);
    return visitados;
}

void lista_gen_percorrer(const LISTA_GEN *lista,
                        void (*visita)(ITEM *item, void *ctx), void *ctx) {
    (void) lista_gen_percorrer_limite(lista, 0, visita, ctx);
}

bool lista_gen_remover(LISTA_GEN *lista, const ITEM *item) {
    if (lista == NULL || item == NULL) return false;

    NO *anterior = NULL;
    NO *atual = lista->inicio;
    while (atual != NULL) {
        if (atual->tipo == TIPO_ATOMO && atual->info.atomo == item) {
            if (anterior == NULL) {
                lista->inicio = atual->prox;
            } else {
                anterior->prox = atual->prox;
            }
            if (lista->fim == atual) lista->fim = anterior;
            apagar_no(atual);
            return true;
        }
        if (atual->tipo == TIPO_SUBLISTA &&
            lista_gen_remover(atual->info.sublista, item)) {
            return true;
        }
        anterior = atual;
        atual = atual->prox;
    }
    return false;
}

void lista_gen_apagar(LISTA_GEN **lista) {
    if (lista == NULL || *lista == NULL) return;

    NO *atual = (*lista)->inicio;
    while (atual != NULL) {
        NO *prox = atual->prox;
        apagar_no(atual);
        atual = prox;
    }
    free(*lista);
    *lista = NULL;
}

int lista_gen_get_profundidade(LISTA_GEN *lista) {
    if (lista == NULL) return -1;
    if (lista->inicio == NULL) return 0;

    int maior = 0;
    for (NO *atual = lista->inicio; atual != NULL; atual = atual->prox) {
        if (atual->tipo == TIPO_SUBLISTA) {
            int profundidade = lista_gen_get_profundidade(atual->info.sublista);
            if (profundidade > maior) maior = profundidade;
        }
    }
    return maior + 1;
}
