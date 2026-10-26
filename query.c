#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <stdbool.h>
#include "item.h"
#include "query.h"

/* Formato do arquivo de consulta: um comando por linha, campos separados
 * por '|'. Linhas em branco sao ignoradas; qualquer outra linha invalida
 * (comando desconhecido, numero de campos errado, campo inexistente,
 * operador ou numero invalido, comando repetido) faz query_criar devolver
 * NULL, com a linha e o motivo em stderr. */

typedef enum {
    OP_IGUAL,        // ==
    OP_DIFERENTE,    // !=
    OP_MENOR,        // <
    OP_MENOR_IGUAL,  // <=
    OP_MAIOR,        // >
    OP_MAIOR_IGUAL   // >=
} OPERADOR;

typedef struct filtro_
{
    char *campo;
    TIPO_CAMPO tipo;    //definido no parse por item_campo_tipo();
    OPERADOR op;
    double valor_num;   //usado se tipo == CAMPO_NUMERO;
    bool valor_bool;    //usado se tipo == CAMPO_BOOL;
    char *valor_texto;  //usado se tipo == CAMPO_TEXTO; pode ser "" (ausente);
    struct filtro_ *prox;
}RegraFiltro;

typedef struct sim_
{
    char *campo;
    double alvo;
    double tolerancia;  //sempre > 0, garantido no parse;
    double peso;        //sempre >= 0, garantido no parse;
    struct sim_ *prox;
}RegraSim;

struct query_
{
    RegraFiltro *regra_filtro, *fim_filtro; //fim: anexar em O(1);
    RegraSim *regra_sim, *fim_sim;

    double sim_aceita;
    bool tem_accept;

    //NULL quando o comando nao aparece; os getters devolvem "" nesse caso;
    char *order_campo;
    char *order_direcao;
    char *group_campo1, *group_campo2;
    char *cross_campo1, *cross_campo2;
};

/* --- FUNCOES AUXILIARES DE TEXTO --- */

static char *copiar_texto(const char *s){
    size_t tamanho = strlen(s) + 1;
    char *copia = (char*)malloc(tamanho);
    if(copia != NULL) memcpy(copia, s, tamanho);
    return copia;
}

/* Le uma linha inteira, de qualquer tamanho, sem o '\n' nem o '\r' final.
 * fgets le em pedacos de ate sizeof(pedaco)-1 caracteres; os pedacos sao
 * concatenados num buffer que cresce ate encontrar o '\n' ou o fim do arquivo.
 * Retorna 1 (linha lida em *linha), 0 (fim do arquivo) ou -1 (falta de memoria). */
static int ler_linha(FILE *arquivo, char **linha){
    char pedaco[256];
    char *buffer = NULL;
    size_t tamanho = 0;

    *linha = NULL;
    while(fgets(pedaco, sizeof(pedaco), arquivo) != NULL){
        size_t n = strlen(pedaco);
        char *novo = (char*)realloc(buffer, tamanho + n + 1);
        if(novo == NULL){
            free(buffer);
            return -1;
        }
        buffer = novo;
        memcpy(buffer + tamanho, pedaco, n + 1);
        tamanho += n;

        if(tamanho > 0 && buffer[tamanho - 1] == '\n') break; //linha completa;
    }
    if(buffer == NULL) return 0; //nada lido: fim do arquivo;

    while(tamanho > 0 && (buffer[tamanho - 1] == '\n' || buffer[tamanho - 1] == '\r')){
        buffer[--tamanho] = '\0';
    }
    *linha = buffer;
    return 1;
}

//remove espacos e tabulacoes do inicio e do fim, no proprio buffer;
static char *aparar(char *s){
    while(*s == ' ' || *s == '\t') s++;

    size_t n = strlen(s);
    while(n > 0 && (s[n - 1] == ' ' || s[n - 1] == '\t')) s[--n] = '\0';
    return s;
}

/* Divide a linha em campos separados por '|', no proprio buffer, e apara
 * cada um. Campos vazios sao preservados ("A||B" tem 3 campos). Guarda ate
 * max campos e retorna quantos existem de fato (pode ser maior que max). */
static size_t separar_campos(char *linha, char *campos[], size_t max){
    size_t n = 0;
    char *inicio = linha;

    for(;;){
        char *barra = strchr(inicio, '|');
        if(barra != NULL) *barra = '\0';
        if(n < max) campos[n] = aparar(inicio);
        n++;
        if(barra == NULL) break;
        inicio = barra + 1;
    }
    return n;
}

//converte o texto inteiro em numero finito; "", "12abc" e "nan" sao invalidos;
static bool ler_numero(const char *texto, double *valor){
    if(*texto == '\0') return false;

    char *fim;
    *valor = strtod(texto, &fim);
    return *fim == '\0' && isfinite(*valor);
}

static bool ler_operador(const char *texto, OPERADOR *op){
    static const struct { const char *simbolo; OPERADOR op; } operadores[] = {
        {"==", OP_IGUAL}, {"!=", OP_DIFERENTE}, {"<", OP_MENOR},
        {"<=", OP_MENOR_IGUAL}, {">", OP_MAIOR}, {">=", OP_MAIOR_IGUAL}
    };
    for(size_t i = 0; i < sizeof(operadores) / sizeof(operadores[0]); i++){
        if(strcmp(texto, operadores[i].simbolo) == 0){
            *op = operadores[i].op;
            return true;
        }
    }
    return false;
}

//mensagem de erro com o numero da linha; sempre retorna false para encerrar o parse;
static bool erro(size_t num_linha, const char *formato, ...){
    va_list args;
    va_start(args, formato);
    fprintf(stderr, "Consulta invalida (linha %lu): ", (unsigned long) num_linha);
    vfprintf(stderr, formato, args);
    fprintf(stderr, "\n");
    va_end(args);
    return false;
}

/* --- UM TRATADOR POR COMANDO ---
 * Recebem os campos ja separados e aparados (campos[0] e o comando) e o
 * numero de campos ja conferido em processar_linha(). */

static bool ler_filter(QUERY *q, char *campos[], size_t num_linha){
    TIPO_CAMPO tipo = item_campo_tipo(campos[1]);
    if(tipo == CAMPO_DESCONHECIDO) return erro(num_linha, "campo '%s' nao existe", campos[1]);

    OPERADOR op;
    if(!ler_operador(campos[2], &op)) return erro(num_linha, "operador '%s' invalido (use == != < <= > >=)", campos[2]);
    if(tipo != CAMPO_NUMERO && op != OP_IGUAL && op != OP_DIFERENTE){
        return erro(num_linha, "campo '%s' nao e numerico: so aceita == e !=", campos[1]);
    }

    RegraFiltro *filtro = (RegraFiltro*)calloc(1, sizeof(RegraFiltro));
    if(filtro == NULL) return erro(num_linha, "memoria insuficiente");
    filtro->tipo = tipo;
    filtro->op = op;

    bool ok = true;
    if(tipo == CAMPO_NUMERO){
        ok = ler_numero(campos[3], &filtro->valor_num) || erro(num_linha, "valor '%s' nao e numero", campos[3]);
    }
    else if(tipo == CAMPO_BOOL){
        if(strcmp(campos[3], "true") == 0 || strcmp(campos[3], "True") == 0) filtro->valor_bool = true;
        else if(strcmp(campos[3], "false") == 0 || strcmp(campos[3], "False") == 0) filtro->valor_bool = false;
        else ok = erro(num_linha, "valor '%s' nao e true/false", campos[3]);
    }
    else{
        filtro->valor_texto = copiar_texto(campos[3]); //qualquer tamanho; "" e valido;
        ok = filtro->valor_texto != NULL || erro(num_linha, "memoria insuficiente");
    }

    filtro->campo = ok ? copiar_texto(campos[1]) : NULL;
    if(!ok || filtro->campo == NULL){
        if(ok) erro(num_linha, "memoria insuficiente");
        free(filtro->valor_texto);
        free(filtro);
        return false;
    }

    if(q->fim_filtro == NULL) q->regra_filtro = filtro;
    else q->fim_filtro->prox = filtro;
    q->fim_filtro = filtro;
    return true;
}

static bool ler_sim(QUERY *q, char *campos[], size_t num_linha){
    if(item_campo_tipo(campos[1]) != CAMPO_NUMERO){
        return erro(num_linha, "SIM exige campo numerico; '%s' nao e", campos[1]);
    }

    double alvo, tolerancia, peso;
    if(!ler_numero(campos[2], &alvo)) return erro(num_linha, "alvo '%s' nao e numero", campos[2]);
    if(!ler_numero(campos[3], &tolerancia) || tolerancia <= 0.0){
        return erro(num_linha, "tolerancia '%s' deve ser numero maior que 0", campos[3]);
    }
    if(!ler_numero(campos[4], &peso) || peso < 0.0){
        return erro(num_linha, "peso '%s' deve ser numero maior ou igual a 0", campos[4]);
    }

    RegraSim *sim = (RegraSim*)calloc(1, sizeof(RegraSim));
    if(sim == NULL) return erro(num_linha, "memoria insuficiente");
    sim->campo = copiar_texto(campos[1]);
    if(sim->campo == NULL){
        free(sim);
        return erro(num_linha, "memoria insuficiente");
    }
    sim->alvo = alvo;
    sim->tolerancia = tolerancia;
    sim->peso = peso;

    if(q->fim_sim == NULL) q->regra_sim = sim;
    else q->fim_sim->prox = sim;
    q->fim_sim = sim;
    return true;
}

static bool ler_accept(QUERY *q, char *campos[], size_t num_linha){
    if(q->tem_accept) return erro(num_linha, "ACCEPT repetido");
    if(!ler_numero(campos[1], &q->sim_aceita)) return erro(num_linha, "limiar '%s' nao e numero", campos[1]);
    q->tem_accept = true;
    return true;
}

static bool ler_order(QUERY *q, char *campos[], size_t num_linha){
    if(q->order_campo != NULL) return erro(num_linha, "ORDER repetido");
    if(strcmp(campos[1], "similarity") != 0 && item_campo_tipo(campos[1]) == CAMPO_DESCONHECIDO){
        return erro(num_linha, "campo '%s' nao existe (use um campo ou similarity)", campos[1]);
    }
    if(strcmp(campos[2], "ASC") != 0 && strcmp(campos[2], "DESC") != 0){
        return erro(num_linha, "direcao '%s' invalida (use ASC ou DESC)", campos[2]);
    }

    q->order_campo = copiar_texto(campos[1]);
    q->order_direcao = copiar_texto(campos[2]);
    if(q->order_campo == NULL || q->order_direcao == NULL) return erro(num_linha, "memoria insuficiente");
    return true;
}

//GROUP e CROSS tem o mesmo formato: dois campos existentes;
static bool ler_par(char **c1, char **c2, const char *comando, char *campos[], size_t num_linha){
    if(*c1 != NULL) return erro(num_linha, "%s repetido", comando);
    for(size_t i = 1; i <= 2; i++){
        if(item_campo_tipo(campos[i]) == CAMPO_DESCONHECIDO){
            return erro(num_linha, "campo '%s' nao existe", campos[i]);
        }
    }

    *c1 = copiar_texto(campos[1]);
    *c2 = copiar_texto(campos[2]);
    if(*c1 == NULL || *c2 == NULL) return erro(num_linha, "memoria insuficiente");
    return true;
}

/* Interpreta uma linha ja sem o '\n'. Confere o comando e o numero de
 * campos antes de chamar o tratador, que so precisa validar o conteudo. */
static bool processar_linha(QUERY *q, char *linha, size_t num_linha){
    static const struct { const char *nome; size_t campos; } comandos[] = {
        {"FILTER", 4}, {"SIM", 5}, {"ACCEPT", 2},
        {"ORDER", 3}, {"GROUP", 3}, {"CROSS", 3}
    };
    enum { MAX_CAMPOS = 5 };

    if(*aparar(linha) == '\0') return true; //linha em branco;

    char *campos[MAX_CAMPOS];
    size_t n = separar_campos(linha, campos, MAX_CAMPOS);

    size_t i = 0;
    size_t qtd = sizeof(comandos) / sizeof(comandos[0]);
    while(i < qtd && strcmp(campos[0], comandos[i].nome) != 0) i++;
    if(i == qtd){
        return erro(num_linha, "comando '%s' desconhecido (use FILTER, SIM, ACCEPT, ORDER, GROUP ou CROSS)", campos[0]);
    }
    if(n != comandos[i].campos){
        return erro(num_linha, "%s espera %lu campos separados por '|', mas a linha tem %lu",
                    comandos[i].nome, (unsigned long) comandos[i].campos, (unsigned long) n);
    }

    switch(i){
        case 0: return ler_filter(q, campos, num_linha);
        case 1: return ler_sim(q, campos, num_linha);
        case 2: return ler_accept(q, campos, num_linha);
        case 3: return ler_order(q, campos, num_linha);
        case 4: return ler_par(&q->group_campo1, &q->group_campo2, "GROUP", campos, num_linha);
        default: return ler_par(&q->cross_campo1, &q->cross_campo2, "CROSS", campos, num_linha);
    }
}

QUERY *query_criar(const char *dados_query){
    if(dados_query == NULL) return NULL;

    QUERY *query = (QUERY*)calloc(1, sizeof(QUERY));
    if(query == NULL) return NULL;

    FILE *texto_query = fopen(dados_query, "r");
    if(texto_query == NULL){
        free(query);
        return NULL;
    }

    char *linha;
    size_t num_linha = 0;
    bool ok = true;
    int estado = 0;

    while(ok && (estado = ler_linha(texto_query, &linha)) == 1){
        num_linha++;
        ok = processar_linha(query, linha, num_linha);
        free(linha);
    }
    if(ok && estado < 0) ok = erro(num_linha + 1, "memoria insuficiente");
    if(ok && ferror(texto_query)) ok = erro(num_linha + 1, "falha de leitura do arquivo");

    fclose(texto_query);
    if(!ok){
        query_apagar(&query);
        return NULL;
    }
    return query;
}

/* --- AVALIACAO --- */

//NaN (valor ausente) torna toda comparacao falsa, exceto !=;
static bool comparar_numero(double valor, OPERADOR op, double referencia){
    switch(op){
        case OP_IGUAL:        return valor == referencia;
        case OP_DIFERENTE:    return valor != referencia;
        case OP_MENOR:        return valor < referencia;
        case OP_MENOR_IGUAL:  return valor <= referencia;
        case OP_MAIOR:        return valor > referencia;
        case OP_MAIOR_IGUAL:  return valor >= referencia;
    }
    return false;
}

static bool filtro_aceita(const RegraFiltro *f, const ITEM *item){
    switch(f->tipo){
        case CAMPO_NUMERO:
            return comparar_numero(item_get_campo_num(item, f->campo), f->op, f->valor_num);
        case CAMPO_BOOL: {
            bool iguais = item_get_campo_bool(item, f->campo) == f->valor_bool;
            return f->op == OP_IGUAL ? iguais : !iguais;
        }
        case CAMPO_TEXTO: {
            const char *valor = item_get_campo_texto(item, f->campo);
            bool iguais = strcmp(valor != NULL ? valor : "", f->valor_texto) == 0; //ausente vale "";
            return f->op == OP_IGUAL ? iguais : !iguais;
        }
        default:
            return false;
    }
}

bool query_avaliar(ITEM *item, QUERY *query, double *sim){
    if(query == NULL || item == NULL) return false;

    //FILTER: todos sao obrigatorios;
    for(RegraFiltro *f = query->regra_filtro; f != NULL; f = f->prox){
        if(!filtro_aceita(f, item)) return false;
    }

    //SIM: media ponderada de s(x, q, t) = max(0, 1 - |x - q| / t);
    double sim_total = 1.0;
    if(query->regra_sim != NULL){
        double soma_ponderada = 0.0;
        double soma_pesos = 0.0;

        for(RegraSim *s = query->regra_sim; s != NULL; s = s->prox){
            double x = item_get_campo_num(item, s->campo);

            double s_i = 0.0; //valor ausente (NaN) contribui com similaridade 0;
            if(!isnan(x)){
                s_i = 1.0 - fabs(x - s->alvo) / s->tolerancia;
                if(s_i < 0.0) s_i = 0.0;
            }
            soma_ponderada += s_i * s->peso;
            soma_pesos += s->peso;
        }
        sim_total = (soma_pesos > 0.0) ? soma_ponderada / soma_pesos : 0.0;
    }

    if(sim != NULL){
        *sim = sim_total;
    }

    return sim_total >= query->sim_aceita;
}

void query_apagar(QUERY **query){
    if(query == NULL || *query == NULL) return;

    RegraFiltro *f = (*query)->regra_filtro;
    while(f != NULL){
        RegraFiltro *apaga = f;
        f = f->prox;
        free(apaga->campo);
        free(apaga->valor_texto);
        free(apaga);
    }

    RegraSim *s = (*query)->regra_sim;
    while(s != NULL){
        RegraSim *apaga = s;
        s = s->prox;
        free(apaga->campo);
        free(apaga);
    }

    free((*query)->order_campo);
    free((*query)->order_direcao);
    free((*query)->group_campo1);
    free((*query)->group_campo2);
    free((*query)->cross_campo1);
    free((*query)->cross_campo2);

    free(*query);
    *query = NULL;
}

/* --- GETTERS ---
 * Devolvem "" quando o comando nao aparece na consulta, para que quem
 * chama possa usar strcmp sem testar NULL; NULL so se query for NULL. */

static const char *ou_vazio(const char *s){
    return s != NULL ? s : "";
}

const char *query_get_cross_campo1(QUERY *query){
    if(query == NULL) return NULL;

    return ou_vazio(query->cross_campo1);
}

const char *query_get_cross_campo2(QUERY *query){
    if(query == NULL) return NULL;

    return ou_vazio(query->cross_campo2);
}

const char *query_get_group_campo1(QUERY *query){
    if(query == NULL) return NULL;

    return ou_vazio(query->group_campo1);
}

const char *query_get_group_campo2(QUERY *query){
    if(query == NULL) return NULL;

    return ou_vazio(query->group_campo2);
}

const char *query_get_order_campo(QUERY *query){
    if(query == NULL) return NULL;

    return ou_vazio(query->order_campo);
}

const char *query_get_order_direcao(QUERY *query){
    if(query == NULL) return NULL;

    return ou_vazio(query->order_direcao);
}
