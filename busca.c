#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tempo.h"
#include "item.h"
#include "query.h"
#include "estrutura.h"
#include "busca.h"

typedef struct {
    QUERY *query;
    ESTRUTURA *resultado;
    size_t aceitos;     //insercoes bem-sucedidas no resultado;
    double insercao_ms; //tempo acumulado so das insercoes;
    bool erro;          //alguma insercao falhou;
} ContextoBusca;

static void processa_item(ITEM *item, void *ctx){
    ContextoBusca *c = (ContextoBusca*) ctx;
    double sim = 0.0;

    if(item == NULL || c == NULL) return;

    if(query_avaliar(item, c->query, &sim)){
        //mede a insercao separadamente para relatar SEARCH_MS e RESULT_INSERT_MS;
        double inicio = tempo_agora_ms();
        bool ok = estrutura_inserir(c->resultado, item, sim);
        c->insercao_ms += tempo_agora_ms() - inicio;

        if(ok){
            c->aceitos++;
        }
        else{
            c->erro = true;
        }
    }
    return;
}

bool buscar(ESTRUTURA *base, QUERY *query, ESTRUTURA *resultado,
            ESTATISTICAS_BUSCA *est){
    if(est != NULL){
        est->encontrados = 0;
        est->busca_ms = 0.0;
        est->insercao_ms = 0.0;
    }
    if(base == NULL || query == NULL || resultado == NULL) return false;

    ContextoBusca contexto = {0};
    contexto.query = query;
    contexto.resultado = resultado;

    //limite 0: a busca percorre a base inteira; top-k so vale na saida;
    double inicio = tempo_agora_ms();
    estrutura_percorrer(base, 0, processa_item, &contexto);
    double total_ms = tempo_agora_ms() - inicio;

    if(est != NULL){
        est->encontrados = contexto.aceitos;
        est->insercao_ms = contexto.insercao_ms;
        est->busca_ms = total_ms - contexto.insercao_ms;
    }
    return !contexto.erro;
}
