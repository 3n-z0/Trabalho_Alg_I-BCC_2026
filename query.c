#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "item.h"
#include "query.h"

typedef struct
{
    char campo[32];
    char ope[3];
    char alvo[32];
}RegraFiltro;

typedef struct
{
    char campo[32];
    float alvo;
    float tolerancia;
    float peso;
}RegraSim;

struct query_
{
    RegraFiltro *regra_filtro;
    int qtd_filtros;

    RegraSim *regra_sim;
    int qtd_sim;

    float sim_aceita;

    char order_campo[32];
    char order_direcao[5];

    char group_campo1[32];
    char group_campo2[32];

    char cross_campo1[32];  
    char cross_campo2[32];
};

static double extrair_valor(ITEM *item, const char *campo){
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

QUERY *query_criar(const char *dados_query){
    if(dados_query != NULL) return NULL;

    QUERY *query = (QUERY*)malloc(sizeof(QUERY));
    if(query == NULL) return NULL;
    query->qtd_filtros = 0;
    query->qtd_sim = 0;

    RegraFiltro *filtro_temp = NULL;
    RegraSim *sim_temp = NULL;

    query->regra_filtro = (RegraFiltro*)malloc(sizeof(RegraFiltro));
    if(query->regra_filtro = NULL) return NULL;

    query->regra_sim = (RegraSim*)malloc(sizeof(RegraSim));
    if(query->regra_sim == NULL) return NULL;

    char campo_query[10];

    FILE *texto_query = fopen(dados_query,"r");
    if(texto_query == NULL) return NULL;

    while(fscanf(texto_query, "%[^|]|", campo_query) != EOF){
        
    }
    
}

bool query_avaliar(QUERY *query, ITEM *item, int *sim){
    if(query = NULL || item == NULL) return false;

}


