#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "item.h"
#include "query.h"

typedef struct filtro_
{
    char campo[64];
    char operador[4];
    double valor_num;
    bool valor_bool;
    char valor_str[64];
    bool eh_numerico;
    bool eh_booleano;
    struct filtro_ *prox;
}RegraFiltro;

typedef struct sim_
{
    char campo[32];
    float alvo;
    float tolerancia;
    float peso;
    struct sim_ *prox;
}RegraSim;

struct query_
{
    RegraFiltro *regra_filtro;

    RegraSim *regra_sim;

    float sim_aceita;

    char order_campo[32];
    char order_direcao[5];

    char group_campo1[32];
    char group_campo2[32];

    char cross_campo1[32];  
    char cross_campo2[32];
};

static double extrair_valor_numerico(ITEM *item, const char *campo){
    if(item == NULL || campo == NULL) return 0.0;

    if (strcmp(campo, "distance_au") == 0) return item_get_distance_au(item);
    if (strcmp(campo, "velocity_km_s") == 0) return item_get_velocity_km_s(item);
    if (strcmp(campo, "risk_score") == 0) return item_get_risk_score(item);
    if (strcmp(campo, "year") == 0) return (double) item_get_year(item);
    if (strcmp(campo, "month") == 0) return (double) item_get_month(item);
    if (strcmp(campo, "absolute_magnitude") == 0) return (double) item_get_absolute_magnitude(item);
    if (strcmp(campo, "panic_level") == 0) return (double) item_get_panic_level(item);

    return 0.0;
}

static bool extrair_valor_booleano(const ITEM *item, const char *campo) {
    if (item == NULL || campo == NULL) return false;

    if (strcmp(campo, "is_future_event") == 0) return item_get_is_future_event(item);
    if (strcmp(campo, "is_past_event") == 0)   return item_get_is_past_event(item);
    if (strcmp(campo, "on_sentry_list") == 0)  return item_get_on_sentry_list(item);

    return false;
}

static const char *extrair_valor_string(const ITEM *item, const char *campo){
    if (item == NULL || campo == NULL) return  NULL;

    if (strcmp(campo, "threat_category") == 0) return item_get_threat_category(item);
    if (strcmp(campo, "panic_verdict") == 0) return item_get_panic_verdict(item);
    if (strcmp(campo, "asteroid_designation") == 0) return item_get_asteroid_designation(item);
    if (strcmp(campo, "asteroid_fullname") == 0) return item_get_asteroid_fullname(item);
    if (strcmp(campo, "close_approach_date") == 0) return item_get_close_approach_date(item);

    return NULL;
}

QUERY *query_criar(const char *dados_query){
    if(dados_query == NULL) return NULL;

    QUERY *query = (QUERY*)calloc(1, sizeof(QUERY));
    if(query == NULL) return NULL;

    query->regra_filtro = NULL;

    query->regra_sim = NULL;

    char campo_query[11];

    FILE *texto_query = fopen(dados_query,"r");
    if(texto_query == NULL) {free(query); return NULL;}

    while(fscanf(texto_query, " %10[^|]|", campo_query) == 1){
        if(strcmp(campo_query, "FILTER") == 0){
            RegraFiltro *filtro = (RegraFiltro*)calloc(1, sizeof(RegraFiltro));
            
            if(filtro == NULL) {query_apagar(&query); fclose(texto_query); return NULL;}
            filtro->prox = NULL;

            fscanf(texto_query, " %[^|]|", filtro->campo);
            fscanf(texto_query, " %[^|]|", filtro->operador);
            fscanf(texto_query, " %[^\r\n]\n", filtro->valor_str);

            if(strcmp(filtro->campo, "distance_au") == 0 || strcmp(filtro->campo, "velocity_km_s") == 0 || strcmp(filtro->campo, "risk_score") == 0 || strcmp(filtro->campo, "year") == 0|| strcmp(filtro->campo, "month") == 0 || strcmp(filtro->campo, "absolute_magnitude") == 0 || strcmp(filtro->campo, "panic_level") == 0){
                filtro->eh_numerico = true;
                filtro->valor_num = atof(filtro->valor_str);
            }
            else if(strcmp(filtro->campo, "is_future_event") == 0 || strcmp(filtro->campo, "is_past_event") == 0 || strcmp(filtro->campo, "on_sentry_list") == 0){
                filtro->eh_booleano = true;
                if(strcmp(filtro->valor_str, "true") == 0){
                    filtro->valor_bool = true;
                }
                else{
                    filtro->valor_bool = false;
                }
            }
            if(query->regra_filtro == NULL){
                query->regra_filtro = filtro;
            }
            else{
                RegraFiltro *atual = query->regra_filtro;
                while(atual->prox != NULL){
                    atual = atual->prox;
                }
                atual->prox = filtro;
            }
        }
        else if(strcmp(campo_query, "SIM") == 0){
            RegraSim *sim = (RegraSim*)calloc(1, sizeof(RegraSim));

            if(sim == NULL) {query_apagar(&query); fclose(texto_query); return NULL;}
            sim->prox = NULL;

            fscanf(texto_query, " %[^|]|", sim->campo);
            fscanf(texto_query, " %f|", &sim->alvo);
            fscanf(texto_query, " %f|", &sim->tolerancia);
            fscanf(texto_query, " %f\n", &sim->peso);

            if(query->regra_sim == NULL){
                query->regra_sim = sim;
            }
            else{
                RegraSim *atual = query->regra_sim;
                
                while(atual->prox != NULL){
                    atual = atual->prox;
                }
                atual->prox = sim;
            }
        }
        else if(strcmp(campo_query, "ACCEPT") == 0){
            fscanf(texto_query, " %f\n", &query->sim_aceita);
        }
        else if(strcmp(campo_query, "ORDER") == 0){
            fscanf(texto_query, " %[^|]|", query->order_campo);
            fscanf(texto_query, " %[^\r\n]\n", query->order_direcao);
        }
        else if(strcmp(campo_query, "GROUP") == 0){
            fscanf(texto_query, " %[^|]|", query->group_campo1);
            fscanf(texto_query, " %[^\r\n]\n", query->group_campo2);
        }
        else if(strcmp(campo_query, "CROSS") == 0){
            fscanf(texto_query, " %[^|]|", query->cross_campo1);
            fscanf(texto_query, " %[^\r\n]\n", query->cross_campo2);
        }
    }
    fclose(texto_query);
    return query;
}

bool query_avaliar(QUERY *query, ITEM *item, float *sim){
    if(query == NULL || item == NULL) return false;

    RegraFiltro *regra_atual = query->regra_filtro;
    while(regra_atual != NULL){
        if(regra_atual->eh_booleano){
            bool valor = extrair_valor_booleano(item, regra_atual->campo);

            if(strcmp(regra_atual->operador, "==") == 0 && valor != regra_atual->valor_bool) return false;
            if(strcmp(regra_atual->operador, "!=") == 0 && valor == regra_atual->valor_bool) return false;
        }
        else if(regra_atual->eh_numerico){
            double valor = extrair_valor_numerico(item, regra_atual->campo);

            if(strcmp(regra_atual->operador, ">=") == 0 && valor < regra_atual->valor_num) return false;
            if(strcmp(regra_atual->operador, "<=") == 0 && valor > regra_atual->valor_num) return false;
            if(strcmp(regra_atual->operador, "!=") == 0 && valor == regra_atual->valor_num) return false;
            if(strcmp(regra_atual->operador, "==") == 0 && valor != regra_atual->valor_num) return false;
            if(strcmp(regra_atual->operador, ">") == 0 && valor <= regra_atual->valor_num) return false;
            if(strcmp(regra_atual->operador, "<") == 0 && valor >= regra_atual->valor_num) return false;

        }
        else{
            const char *valor = extrair_valor_string(item, regra_atual->campo);

            if(valor == NULL) return false;

            if(strcmp(regra_atual->operador, "==") == 0 && strcmp(regra_atual->valor_str, valor) != 0) return false;
            if(strcmp(regra_atual->operador, "!=") == 0 && strcmp(regra_atual->valor_str, valor) == 0) return false;

        }
        regra_atual = regra_atual->prox;
    }

    double sim_total = 1.0;

    if(query->regra_sim != NULL){
        double soma_ponderada = 0.0;
        double soma_pesos = 0.0;

        RegraSim *sim_atual = query->regra_sim;
        while(sim_atual != NULL){
            double x = extrair_valor_numerico(item, sim_atual->campo);

            double s_i = 0.0;
            if(sim_atual->tolerancia > 0.0){
                double dif = fabs(x - sim_atual->alvo);
                s_i = 1.0 - (dif/sim_atual->tolerancia);
                if(s_i < 0.0) s_i = 0.0;
            }

            soma_ponderada += (s_i*sim_atual->peso);
            soma_pesos += sim_atual->peso;

            sim_atual = sim_atual->prox;
        }

        sim_total = (soma_pesos > 0.0) ? soma_ponderada/soma_pesos : 0.0;
    }

    if(sim != NULL){
        *sim = sim_total;
    }

    return (bool)(sim_total >= query->sim_aceita);
}

void query_apagar(QUERY **query){
    if(query == NULL || *query == NULL) return;

    RegraFiltro *atualf = (*query)->regra_filtro;
    RegraFiltro *apagaf = NULL;
    (*query)->regra_filtro = NULL;

    while(atualf != NULL){
        apagaf = atualf;
        atualf = atualf->prox;
        free(apagaf);
    }

    RegraSim *atuals = (*query)->regra_sim;
    RegraSim *apagas = NULL;

    while(atuals != NULL){
        apagas = atuals;
        atuals = atuals->prox;
        free(apagas);
    }

    free(*query);
    *query = NULL;
}

