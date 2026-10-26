#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "busca.h"
#include "leitor_nasa.h"
#include "tempo.h"

/* A main coordena o fluxo; cada modulo conhece sua parte do trabalho. */
typedef struct {
    const char *reader, *input, *base, *result, *query, *output;
    size_t top_k, limit;
} Opcoes;

static void uso(FILE *saida, const char *programa) {
    fprintf(saida,
        "Uso: %s --reader nasa --input dados.csv --base ESTRUTURA\n"
        "       --result ESTRUTURA --query consulta.txt --output resultado.csv\n"
        "       [--top-k N] [--limit N]\n\n"
        "Estruturas: pilha, fila, lista_seq, lista_enc, lista_cabeca,\n"
        "            lista_ord, lista_gen, lista_cruz.\n"
        "top-k e limit: inteiros nao negativos; 0 significa todos (padrao).\n",
        programa);
}

static bool ler_quantidade(const char *texto, size_t *valor) {
    if (!texto || !*texto || !valor) return false;
    size_t numero = 0;
    /* size_t e sem sinal: converter -1 produz seu maior valor representavel. */
    const size_t maximo = (size_t)-1;

    for (const char *p = texto; *p; p++) {
        if (*p < '0' || *p > '9') return false;
        size_t digito = (size_t)(*p - '0');
        /* Verifica o limite ANTES de multiplicar por 10 e somar o digito. */
        if (numero > (maximo - digito) / 10) return false;
        numero = numero * 10 + digito;
    }
    *valor = numero;
    return true;
}

static bool ler_argumentos(int argc, char **argv, Opcoes *opcoes) {
    static const char *nomes[] = {"--reader", "--input", "--base", "--result",
        "--query", "--output", "--top-k", "--limit"};
    *opcoes = (Opcoes){0};
    const char **textos[] = {&opcoes->reader, &opcoes->input, &opcoes->base,
        &opcoes->result, &opcoes->query, &opcoes->output};
    unsigned int vistos = 0;
    for (int i = 1; i < argc; i += 2) {
        size_t j = 0;
        while (j < 8 && strcmp(argv[i], nomes[j])) j++;
        if (j == 8 || i + 1 == argc || !*argv[i+1] || (vistos & (1u << j))) return false;
        vistos |= 1u << j;
        if (j < 6) *textos[j] = argv[i+1];
        else if (!ler_quantidade(argv[i+1], j == 6 ? &opcoes->top_k : &opcoes->limit)) return false;
    }
    return (vistos & 63u) == 63u; /* As seis opcoes de texto sao obrigatorias. */
}

/* Usada no encerramento normal e em cada retorno apos alocar recursos.
 * As funcoes chamadas aceitam ponteiros NULL, inclusive em criacao parcial. */
static void liberar_recursos(QUERY **query, ESTRUTURA **base,
                             ESTRUTURA **resultado, LEITOR_NASA **leitor) {
    estrutura_apagar(resultado, false);
    estrutura_apagar(base, true);
    leitor_nasa_fechar(leitor);
    query_apagar(query);
}

/* Todo encerramento com erro passa por aqui: a correcao automatica le a
 * saida padrao, entao o STATUS sai em stdout; o motivo ja foi para stderr. */
static int falhar(void) {
    printf("STATUS=ERROR\n");
    return EXIT_FAILURE;
}

int main(int argc, char **argv) {
    if (argc == 2 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))) {
        uso(stdout, argv[0]);
        return EXIT_SUCCESS;
    }

    /* 1. Ler e validar os argumentos antes de abrir arquivos. */
    Opcoes op;
    TipoResultado tipo_base, tipo_resultado;
    if (!ler_argumentos(argc, argv, &op)) {
        fprintf(stderr, "Erro: argumentos ausentes, duplicados ou invalidos.\n");
        uso(stderr, argv[0]);
        return falhar();
    }
    if (strcmp(op.reader, "nasa")) {
        fprintf(stderr, "Erro: leitor '%s' ainda nao implementado. Disponivel: nasa.\n", op.reader);
        return falhar();
    }
    if (!estrutura_tipo(op.base, &tipo_base) || !estrutura_tipo(op.result, &tipo_resultado)) {
        fprintf(stderr, "Erro: nome de estrutura invalido. Consulte --help.\n");
        return falhar();
    }
    if (!strcmp(op.output, op.input) || !strcmp(op.output, op.query)) {
        fprintf(stderr, "Erro: a saida deve ter caminho diferente da entrada e da consulta.\n");
        return falhar();
    }

    QUERY *query = NULL;
    ESTRUTURA *base = NULL, *resultado = NULL;
    LEITOR_NASA *leitor = NULL;
    size_t carregados = 0, gravados = 0;
    ESTATISTICAS_BUSCA estatisticas = {0};

    /* 2. Carregar consulta e criar dois TADs independentes. */
    query = query_criar(op.query);
    if (!query) {
        fprintf(stderr, "Erro: consulta inacessivel, invalida ou falta de memoria: %s\n", op.query);
        liberar_recursos(&query, &base, &resultado, &leitor);
        return falhar();
    }
    base = estrutura_criar(tipo_base, query);
    resultado = estrutura_criar(tipo_resultado, query);
    if (!base || !resultado) {
        fprintf(stderr, "Erro: nao foi possivel criar as estruturas.\n");
        liberar_recursos(&query, &base, &resultado, &leitor);
        return falhar();
    }

    /* 3. Cada registro vai diretamente para a estrutura indicada por --base. */
    double inicio = tempo_agora_ms();
    leitor = leitor_nasa_abrir(op.input);
    if (!leitor) {
        fprintf(stderr, "Erro: arquivo inacessivel, cabecalho NASA invalido ou falta de memoria.\n");
        liberar_recursos(&query, &base, &resultado, &leitor);
        return falhar();
    }
    while (op.limit == 0 || carregados < op.limit) {
        ITEM *item = NULL;
        int estado = leitor_nasa_proximo(leitor, &item);
        if (estado == 0) break;
        if (estado < 0) {
            fprintf(stderr, "Erro ao ler o registro %zu do CSV.\n", carregados + 1);
            liberar_recursos(&query, &base, &resultado, &leitor);
            return falhar();
        }
        double sim = 0.0;
        if (tipo_base == RESULT_LISTA_ORD && !strcmp(query_get_order_campo(query), "similarity"))
            (void)query_avaliar(item, query, &sim);
        if (!estrutura_inserir(base, item, sim)) {
            item_apagar(&item); /* Ainda nao pertence a base. */
            fprintf(stderr, "Erro ao inserir na base: memoria insuficiente.\n");
            liberar_recursos(&query, &base, &resultado, &leitor);
            return falhar();
        }
        carregados++;
    }
    double carga_ms = tempo_agora_ms() - inicio;

    /* 4. Percorrer a base, avaliar a consulta e inserir todos os aprovados. */
    if (!buscar(base, query, resultado, &estatisticas)) {
        fprintf(stderr, "Erro ao armazenar os resultados da busca.\n");
        liberar_recursos(&query, &base, &resultado, &leitor);
        return falhar();
    }

    /* 5. top-k limita somente a exportacao, sem reduzir MATCHED_RECORDS. */
    inicio = tempo_agora_ms();
    if (!leitor_nasa_salvar(leitor, op.output, resultado, op.top_k, &gravados)) {
        fprintf(stderr, "Erro ao gravar a saida: %s\n", op.output);
        liberar_recursos(&query, &base, &resultado, &leitor);
        return falhar();
    }
    double escrita_ms = tempo_agora_ms() - inicio;
    printf("STATUS=OK\nINPUT_RECORDS=%zu\nMATCHED_RECORDS=%zu\nOUTPUT_RECORDS=%zu\n"
           "LOAD_MS=%.3f\nSEARCH_MS=%.3f\nRESULT_INSERT_MS=%.3f\nWRITE_MS=%.3f\n",
           carregados, estatisticas.encontrados, gravados, carga_ms,
           estatisticas.busca_ms, estatisticas.insercao_ms, escrita_ms);
    /* 6. Resultado empresta ITEMs da base; somente a base os libera. */
    liberar_recursos(&query, &base, &resultado, &leitor);
    return EXIT_SUCCESS;
}
