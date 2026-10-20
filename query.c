#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "item.h"

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


