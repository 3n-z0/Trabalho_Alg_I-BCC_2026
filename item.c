#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include "item.h"

typedef struct //struct será alocada exclusivamente se on_sentry_info == true;
{
    float sentry_torino_scale;
    float sentry_palermo_scale;
    float sentry_diameter_km;
    char *sentry_impact_prob;
}SentryInfo;


struct item_
{
    //8 bytes;
    double risk_score; 
    double distance_au;
    double velocity_km_s;
    SentryInfo *sentry_info; //será NULL se on_sentry_info == false;
    char *linha_original;    //linha do CSV, gravada sem alteracao na saida; NULL ate ser definida;

    //textos alocados no tamanho exato do valor lido (sem limite de tamanho);
    char *threat_category;
    char *panic_verdict;
    char *asteroid_designation;
    char *asteroid_fullname;
    char *close_approach_date;

    //4 bytes;
    unsigned int year, month;
    unsigned int panic_level;
    int days_until_approach;
    float absolute_magnitude;

    //1 byte;
    bool on_sentry_list;
    bool is_past_event;
    bool is_future_event;
};

//copia do texto no tamanho exato; NULL vira "" para os getters nunca devolverem NULL;
static char *copiar_texto(const char *s){
    if(s == NULL) s = "";
    size_t tamanho = strlen(s) + 1;
    char *copia = (char*)malloc(tamanho);
    if(copia != NULL) memcpy(copia, s, tamanho);
    return copia;
}

ITEM *item_criar(
const char *asteroid_designation,
const char *asteroid_fullname,
const char *close_approach_date,
unsigned int year,
unsigned int month,
double distance_au,
double velocity_km_s,
float absolute_magnitude,
int days_until_approach,
bool is_past_event,
bool is_future_event,
double risk_score,
unsigned int panic_level,
const char *threat_category,
const char *panic_verdict,
bool on_sentry_list,
const char *sentry_impact_prob,
float sentry_torino_scale,
float sentry_palermo_scale,
float sentry_diameter_km){
    //calloc: ponteiros comecam NULL, entao item_apagar libera com seguranca
    //um ITEM criado pela metade, se alguma alocacao abaixo falhar;
    ITEM *item = (ITEM*)calloc(1, sizeof(ITEM));
    if(item == NULL){
        return NULL;
    }

    if(on_sentry_list){ //atribui características condicionadas a estar na sentry list;
        item->sentry_info = (SentryInfo*)calloc(1, sizeof(SentryInfo));
        if(item->sentry_info == NULL){
            item_apagar(&item);
            return NULL;
        }
        item->sentry_info->sentry_torino_scale = sentry_torino_scale;
        item->sentry_info->sentry_palermo_scale = sentry_palermo_scale;
        item->sentry_info->sentry_diameter_km = sentry_diameter_km;
        item->sentry_info->sentry_impact_prob = copiar_texto(sentry_impact_prob);
    }

    item->asteroid_designation = copiar_texto(asteroid_designation);
    item->asteroid_fullname = copiar_texto(asteroid_fullname);
    item->close_approach_date = copiar_texto(close_approach_date);
    item->threat_category = copiar_texto(threat_category);
    item->panic_verdict = copiar_texto(panic_verdict);

    if(item->asteroid_designation == NULL || item->asteroid_fullname == NULL ||
       item->close_approach_date == NULL || item->threat_category == NULL ||
       item->panic_verdict == NULL ||
       (item->sentry_info != NULL && item->sentry_info->sentry_impact_prob == NULL)){
        item_apagar(&item);
        return NULL;
    }

    item->year = year;
    item->month = month;
    item->distance_au = distance_au;
    item->velocity_km_s = velocity_km_s;
    item->absolute_magnitude = absolute_magnitude;
    item->days_until_approach = days_until_approach;
    item->is_past_event = is_past_event;
    item->is_future_event = is_future_event;
    item->risk_score = risk_score;
    item->panic_level = panic_level;
    item->on_sentry_list = on_sentry_list;

    return item;
}

void item_apagar(ITEM **item){
    if(item == NULL || *item == NULL){
        return;
    }
    if((*item)->sentry_info != NULL){
        free((*item)->sentry_info->sentry_impact_prob);
        free((*item)->sentry_info);
    }
    free((*item)->threat_category);
    free((*item)->panic_verdict);
    free((*item)->asteroid_designation);
    free((*item)->asteroid_fullname);
    free((*item)->close_approach_date);
    free((*item)->linha_original);
    free(*item);
    *item = NULL;
}

bool item_set_linha_original(ITEM *item, const char *linha){
    if(item == NULL || linha == NULL){
        return false;
    }
    size_t tamanho = strlen(linha) + 1;
    char *copia = (char*)malloc(tamanho);
    if(copia == NULL){
        return false;
    }
    memcpy(copia, linha, tamanho);

    free(item->linha_original);
    item->linha_original = copia;
    return true;
}

const char *item_get_linha_original(const ITEM *item){
    if(item == NULL) return NULL;
    return item->linha_original;
}

const char *item_get_asteroid_designation(const ITEM *item) {
    if (item == NULL) return NULL;
    return item->asteroid_designation;
}

const char *item_get_asteroid_fullname(const ITEM *item) {
    if (item == NULL) return NULL;
    return item->asteroid_fullname;
}

const char *item_get_close_approach_date(const ITEM *item) {
    if (item == NULL) return NULL;
    return item->close_approach_date;
}

const char *item_get_threat_category(const ITEM *item) {
    if (item == NULL) return NULL;
    return item->threat_category;
}

const char *item_get_panic_verdict(const ITEM *item) {
    if (item == NULL) return NULL;
    return item->panic_verdict;
}

unsigned int item_get_year(const ITEM *item) {
    if (item == NULL) return 0;
    return item->year;
}

unsigned int item_get_month(const ITEM *item) {
    if (item == NULL) return 0;
    return item->month;
}

double item_get_distance_au(const ITEM *item) {
    if (item == NULL) return 0.0;
    return item->distance_au;
}

double item_get_velocity_km_s(const ITEM *item) {
    if (item == NULL) return 0.0;
    return item->velocity_km_s;
}

float item_get_absolute_magnitude(const ITEM *item) {
    if (item == NULL) return 0.0f;
    return item->absolute_magnitude;
}

int item_get_days_until_approach(const ITEM *item) {
    if (item == NULL) return 0;
    return item->days_until_approach;
}

bool item_get_is_past_event(const ITEM *item) {
    if (item == NULL) return false;
    return item->is_past_event;
}

bool item_get_is_future_event(const ITEM *item) {
    if (item == NULL) return false;
    return item->is_future_event;
}

double item_get_risk_score(const ITEM *item) {
    if (item == NULL) return 0.0;
    return item->risk_score;
}

unsigned int item_get_panic_level(const ITEM *item) {
    if (item == NULL) return 0;
    return item->panic_level;
}

bool item_get_on_sentry_list(const ITEM *item) {
    if (item == NULL) return false;
    return item->on_sentry_list;
}

const char *item_get_sentry_impact_prob(const ITEM *item) {
    if (item == NULL || !item->on_sentry_list || item->sentry_info == NULL) {
        return NULL;
    }
    return item->sentry_info->sentry_impact_prob;
}

float item_get_sentry_torino_scale(const ITEM *item) {
    if (item == NULL || !item->on_sentry_list || item->sentry_info == NULL) {
        return 0.0f;
    }
    return item->sentry_info->sentry_torino_scale;
}

float item_get_sentry_palermo_scale(const ITEM *item) {
    if (item == NULL || !item->on_sentry_list || item->sentry_info == NULL) {
        return 0.0f;
    }
    return item->sentry_info->sentry_palermo_scale;
}

float item_get_sentry_diameter_km(const ITEM *item) {
    if (item == NULL || !item->on_sentry_list || item->sentry_info == NULL) {
        return 0.0f;
    }
    return item->sentry_info->sentry_diameter_km;
}

/* --- ACESSO A CAMPOS POR NOME ---
 * Unico lugar do projeto que associa o nome de uma coluna do dataset ao seu
 * tipo e ao getter correspondente. query.c e busca.c usam apenas as funcoes
 * abaixo; para um novo campo, basta acrescenta-lo em ID_CAMPO, em CAMPOS e no
 * switch do getter do seu tipo. */
typedef enum {
    //texto;
    CP_ASTEROID_DESIGNATION,
    CP_ASTEROID_FULLNAME,
    CP_CLOSE_APPROACH_DATE,
    CP_THREAT_CATEGORY,
    CP_PANIC_VERDICT,
    CP_SENTRY_IMPACT_PROB,
    //numericos;
    CP_YEAR,
    CP_MONTH,
    CP_DISTANCE_AU,
    CP_VELOCITY_KM_S,
    CP_ABSOLUTE_MAGNITUDE,
    CP_DAYS_UNTIL_APPROACH,
    CP_RISK_SCORE,
    CP_PANIC_LEVEL,
    CP_SENTRY_TORINO_SCALE,
    CP_SENTRY_PALERMO_SCALE,
    CP_SENTRY_DIAMETER_KM,
    //booleanos;
    CP_IS_PAST_EVENT,
    CP_IS_FUTURE_EVENT,
    CP_ON_SENTRY_LIST,
    CP_QTD
} ID_CAMPO;

typedef struct {
    const char *nome;
    TIPO_CAMPO tipo;
    int casas;       //casas decimais usadas por item_get_campo_str (so numericos);
    bool so_sentry;  //sem valor quando o asteroide nao esta na lista Sentry;
} DESCRITOR_CAMPO;

static const DESCRITOR_CAMPO CAMPOS[CP_QTD] = {
    [CP_ASTEROID_DESIGNATION] = {"asteroid_designation", CAMPO_TEXTO,  0, false},
    [CP_ASTEROID_FULLNAME]    = {"asteroid_fullname",    CAMPO_TEXTO,  0, false},
    [CP_CLOSE_APPROACH_DATE]  = {"close_approach_date",  CAMPO_TEXTO,  0, false},
    [CP_THREAT_CATEGORY]      = {"threat_category",      CAMPO_TEXTO,  0, false},
    [CP_PANIC_VERDICT]        = {"panic_verdict",        CAMPO_TEXTO,  0, false},
    [CP_SENTRY_IMPACT_PROB]   = {"sentry_impact_prob",   CAMPO_TEXTO,  0, true},
    [CP_YEAR]                 = {"year",                 CAMPO_NUMERO, 0, false},
    [CP_MONTH]                = {"month",                CAMPO_NUMERO, 0, false},
    [CP_DISTANCE_AU]          = {"distance_au",          CAMPO_NUMERO, 4, false},
    [CP_VELOCITY_KM_S]        = {"velocity_km_s",        CAMPO_NUMERO, 2, false},
    [CP_ABSOLUTE_MAGNITUDE]   = {"absolute_magnitude",   CAMPO_NUMERO, 2, false},
    [CP_DAYS_UNTIL_APPROACH]  = {"days_until_approach",  CAMPO_NUMERO, 0, false},
    [CP_RISK_SCORE]           = {"risk_score",           CAMPO_NUMERO, 4, false},
    [CP_PANIC_LEVEL]          = {"panic_level",          CAMPO_NUMERO, 0, false},
    [CP_SENTRY_TORINO_SCALE]  = {"sentry_torino_scale",  CAMPO_NUMERO, 1, true},
    [CP_SENTRY_PALERMO_SCALE] = {"sentry_palermo_scale", CAMPO_NUMERO, 2, true},
    [CP_SENTRY_DIAMETER_KM]   = {"sentry_diameter_km",   CAMPO_NUMERO, 3, true},
    [CP_IS_PAST_EVENT]        = {"is_past_event",        CAMPO_BOOL,   0, false},
    [CP_IS_FUTURE_EVENT]      = {"is_future_event",      CAMPO_BOOL,   0, false},
    [CP_ON_SENTRY_LIST]       = {"on_sentry_list",       CAMPO_BOOL,   0, false}
};

//indice do campo em CAMPOS, ou -1 se o nome nao existir;
static int campo_id(const char *campo){
    if (campo == NULL) return -1;

    for (int i = 0; i < CP_QTD; i++) {
        if (strcmp(campo, CAMPOS[i].nome) == 0) return i;
    }
    return -1;
}

static const char *texto_por_id(const ITEM *item, int id){
    switch (id) {
        case CP_ASTEROID_DESIGNATION: return item_get_asteroid_designation(item);
        case CP_ASTEROID_FULLNAME:    return item_get_asteroid_fullname(item);
        case CP_CLOSE_APPROACH_DATE:  return item_get_close_approach_date(item);
        case CP_THREAT_CATEGORY:      return item_get_threat_category(item);
        case CP_PANIC_VERDICT:        return item_get_panic_verdict(item);
        case CP_SENTRY_IMPACT_PROB:   return item_get_sentry_impact_prob(item);
        default:                      return NULL;
    }
}

static double num_por_id(const ITEM *item, int id){
    switch (id) {
        case CP_YEAR:                 return (double) item_get_year(item);
        case CP_MONTH:                return (double) item_get_month(item);
        case CP_DISTANCE_AU:          return item_get_distance_au(item);
        case CP_VELOCITY_KM_S:        return item_get_velocity_km_s(item);
        case CP_ABSOLUTE_MAGNITUDE:   return (double) item_get_absolute_magnitude(item);
        case CP_DAYS_UNTIL_APPROACH:  return (double) item_get_days_until_approach(item);
        case CP_RISK_SCORE:           return item_get_risk_score(item);
        case CP_PANIC_LEVEL:          return (double) item_get_panic_level(item);
        case CP_SENTRY_TORINO_SCALE:  return (double) item_get_sentry_torino_scale(item);
        case CP_SENTRY_PALERMO_SCALE: return (double) item_get_sentry_palermo_scale(item);
        case CP_SENTRY_DIAMETER_KM:   return (double) item_get_sentry_diameter_km(item);
        default:                      return 0.0;
    }
}

static bool bool_por_id(const ITEM *item, int id){
    switch (id) {
        case CP_IS_PAST_EVENT:   return item_get_is_past_event(item);
        case CP_IS_FUTURE_EVENT: return item_get_is_future_event(item);
        case CP_ON_SENTRY_LIST:  return item_get_on_sentry_list(item);
        default:                 return false;
    }
}

TIPO_CAMPO item_campo_tipo(const char *campo){
    int id = campo_id(campo);
    return id < 0 ? CAMPO_DESCONHECIDO : CAMPOS[id].tipo;
}

const char *item_get_campo_texto(const ITEM *item, const char *campo){
    int id = campo_id(campo);
    if (item == NULL || id < 0 || CAMPOS[id].tipo != CAMPO_TEXTO) return NULL;
    return texto_por_id(item, id);
}

double item_get_campo_num(const ITEM *item, const char *campo){
    int id = campo_id(campo);
    if (item == NULL || id < 0 || CAMPOS[id].tipo != CAMPO_NUMERO) return 0.0;
    return num_por_id(item, id);
}

bool item_get_campo_bool(const ITEM *item, const char *campo){
    int id = campo_id(campo);
    if (item == NULL || id < 0 || CAMPOS[id].tipo != CAMPO_BOOL) return false;
    return bool_por_id(item, id);
}

void item_get_campo_str(const ITEM *item, const char *campo, char *dest, size_t tam_dest){
    if (item == NULL || campo == NULL || dest == NULL || tam_dest == 0) return;

    int id = campo_id(campo);
    if (id < 0) {
        snprintf(dest, tam_dest, "DESCONHECIDO");
        return;
    }

    switch (CAMPOS[id].tipo) {
        case CAMPO_TEXTO: {
            const char *val = texto_por_id(item, id);
            snprintf(dest, tam_dest, "%s", val ? val : "N/A");
            break;
        }
        case CAMPO_BOOL:
            snprintf(dest, tam_dest, "%s", bool_por_id(item, id) ? "true" : "false");
            break;
        case CAMPO_NUMERO:
            if (CAMPOS[id].so_sentry && !item_get_on_sentry_list(item)) {
                snprintf(dest, tam_dest, "N/A");
            } else {
                snprintf(dest, tam_dest, "%.*f", CAMPOS[id].casas, num_por_id(item, id));
            }
            break;
        default:
            snprintf(dest, tam_dest, "DESCONHECIDO");
            break;
    }
}
