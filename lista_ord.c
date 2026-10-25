#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "lista_ord.h"

typedef struct no_ord_ {
    ITEM *item;
    union {
        double numero;
        char *texto;
    } chave;
    bool ausente;
    struct no_ord_ *prox;
} NO_ORD;

struct lista_ord_ {
    NO_ORD *inicio;
    NO_ORD *fim;
    size_t tamanho;
    LISTA_ORD_TIPO tipo;
    LISTA_ORD_ORDEM ordem;
};

LISTA_ORD *lista_ord_criar(LISTA_ORD_TIPO tipo, LISTA_ORD_ORDEM ordem) {
    if ((tipo != LISTA_ORD_NUMERICA && tipo != LISTA_ORD_TEXTUAL) ||
        (ordem != LISTA_ORD_ASC && ordem != LISTA_ORD_DESC)) {
        return NULL;
    }

    LISTA_ORD *lista = malloc(sizeof(*lista));
    if (lista == NULL) return NULL;

    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
    lista->tipo = tipo;
    lista->ordem = ordem;
    return lista;
}

static int comparar(const LISTA_ORD *lista, const NO_ORD *a, const NO_ORD *b) {
    if (a->ausente != b->ausente) return a->ausente ? 1 : -1;
    if (a->ausente) return 0;

    int resultado;
    if (lista->tipo == LISTA_ORD_NUMERICA) {
        resultado = (a->chave.numero > b->chave.numero) -
                    (a->chave.numero < b->chave.numero);
    } else {
        int cmp = strcmp(a->chave.texto, b->chave.texto);
        resultado = (cmp > 0) - (cmp < 0);
    }
    return lista->ordem == LISTA_ORD_ASC ? resultado : -resultado;
}

static void inserir_no(LISTA_ORD *lista, NO_ORD *novo) {
    novo->prox = NULL;

    if (lista->fim == NULL || comparar(lista, lista->fim, novo) <= 0) {
        if (lista->fim == NULL) {
            lista->inicio = novo;
        } else {
            lista->fim->prox = novo;
        }
        lista->fim = novo;
    } else {
        NO_ORD **posicao = &lista->inicio;
        while (*posicao != NULL && comparar(lista, *posicao, novo) <= 0) {
            posicao = &(*posicao)->prox;
        }
        novo->prox = *posicao;
        *posicao = novo;
    }
    lista->tamanho++;
}

bool lista_ord_inserir_numero(LISTA_ORD *lista, ITEM *item, double chave) {
    if (lista == NULL || item == NULL || lista->tipo != LISTA_ORD_NUMERICA) {
        return false;
    }

    NO_ORD *novo = malloc(sizeof(*novo));
    if (novo == NULL) return false;

    novo->item = item;
    novo->chave.numero = chave;
    novo->ausente = isnan(chave);
    inserir_no(lista, novo);
    return true;
}

bool lista_ord_inserir_texto(LISTA_ORD *lista, ITEM *item, const char *chave) {
    if (lista == NULL || item == NULL || lista->tipo != LISTA_ORD_TEXTUAL) {
        return false;
    }

    NO_ORD *novo = malloc(sizeof(*novo));
    if (novo == NULL) return false;

    novo->item = item;
    novo->chave.texto = NULL;
    novo->ausente = chave == NULL;
    if (chave != NULL) {
        size_t tamanho = strlen(chave) + 1;
        novo->chave.texto = malloc(tamanho);
        if (novo->chave.texto == NULL) {
            free(novo);
            return false;
        }
        memcpy(novo->chave.texto, chave, tamanho);
    }
    inserir_no(lista, novo);
    return true;
}

static void apagar_no(const LISTA_ORD *lista, NO_ORD *no) {
    if (lista->tipo == LISTA_ORD_TEXTUAL) free(no->chave.texto);
    free(no);
}

ITEM *lista_ord_inicio(const LISTA_ORD *lista) {
    if (lista == NULL || lista->inicio == NULL) return NULL;
    return lista->inicio->item;
}

ITEM *lista_ord_retirar_primeiro(LISTA_ORD *lista) {
    if (lista == NULL || lista->inicio == NULL) return NULL;

    NO_ORD *removido = lista->inicio;
    ITEM *item = removido->item;
    lista->inicio = removido->prox;
    if (lista->inicio == NULL) lista->fim = NULL;
    lista->tamanho--;
    apagar_no(lista, removido);
    return item;
}

bool lista_ord_remover(LISTA_ORD *lista, const ITEM *item) {
    if (lista == NULL || item == NULL) return false;

    NO_ORD *anterior = NULL;
    NO_ORD *atual = lista->inicio;
    while (atual != NULL) {
        if (atual->item == item) {
            if (anterior == NULL) {
                lista->inicio = atual->prox;
            } else {
                anterior->prox = atual->prox;
            }
            if (lista->fim == atual) lista->fim = anterior;
            lista->tamanho--;
            apagar_no(lista, atual);
            return true;
        }
        anterior = atual;
        atual = atual->prox;
    }
    return false;
}

bool lista_ord_vazia(const LISTA_ORD *lista) {
    return lista == NULL || lista->tamanho == 0;
}

size_t lista_ord_tamanho(const LISTA_ORD *lista) {
    return lista == NULL ? 0 : lista->tamanho;
}

size_t lista_ord_percorrer_limite(const LISTA_ORD *lista, size_t limite,
                                void (*visita)(ITEM *item, void *ctx), void *ctx) {
    if (lista == NULL || visita == NULL) return 0;

    size_t visitados = 0;
    for (NO_ORD *atual = lista->inicio; atual != NULL; atual = atual->prox) {
        if (limite != 0 && visitados >= limite) break;
        visita(atual->item, ctx);
        visitados++;
    }
    return visitados;
}

void lista_ord_percorrer(const LISTA_ORD *lista,
                        void (*visita)(ITEM *item, void *ctx), void *ctx) {
    (void) lista_ord_percorrer_limite(lista, 0, visita, ctx);
}

void lista_ord_apagar(LISTA_ORD **lista) {
    if (lista == NULL || *lista == NULL) return;

    NO_ORD *atual = (*lista)->inicio;
    while (atual != NULL) {
        NO_ORD *prox = atual->prox;
        apagar_no(*lista, atual);
        atual = prox;
    }
    free(*lista);
    *lista = NULL;
}
