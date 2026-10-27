#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include "item.h"
#include "item_nasa.h"

typedef struct //struct será alocada exclusivamente se on_sentry_info == true;
{
    float sentry_torino_scale;
    float sentry_palermo_scale;
    float sentry_diameter_km;
    char *sentry_impact_prob;
}SentryInfo;

/* Dados de um asteroide. Fica dentro de um ITEM generico (item.c), que
 * guarda tambem a linha original do CSV. */
typedef struct
{
    //8 bytes;
    double risk_score;
    double distance_au;
    double velocity_km_s;
    SentryInfo *sentry_info; //será NULL se on_sentry_info == false;

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
}ITEM_NASA;

//copia do texto no tamanho exato; NULL vira "" para os getters nunca devolverem NULL;
static char *copiar_texto(const char *s){
    if(s == NULL) s = "";
    size_t tamanho = strlen(s) + 1;
    char *copia = (char*)malloc(tamanho);
    if(copia != NULL) memcpy(copia, s, tamanho);
    return copia;
}

//libera os dados do asteroide; e a funcao apagar_dados de ITEM_OPS_NASA;
static void nasa_apagar(void *dados){
    ITEM_NASA *a = (ITEM_NASA*) dados;
    if(a == NULL){
        return;
    }
    if(a->sentry_info != NULL){
        free(a->sentry_info->sentry_impact_prob);
        free(a->sentry_info);
    }
    free(a->threat_category);
    free(a->panic_verdict);
    free(a->asteroid_designation);
    free(a->asteroid_fullname);
    free(a->close_approach_date);
    free(a);
}

ITEM *item_nasa_criar(
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
    //calloc: ponteiros comecam NULL, entao nasa_apagar libera com seguranca
    //um registro criado pela metade, se alguma alocacao abaixo falhar;
    ITEM_NASA *a = (ITEM_NASA*)calloc(1, sizeof(ITEM_NASA));
    if(a == NULL){
        return NULL;
    }

    if(on_sentry_list){ //atribui características condicionadas a estar na sentry list;
        a->sentry_info = (SentryInfo*)calloc(1, sizeof(SentryInfo));
        if(a->sentry_info == NULL){
            nasa_apagar(a);
            return NULL;
        }
        a->sentry_info->sentry_torino_scale = sentry_torino_scale;
        a->sentry_info->sentry_palermo_scale = sentry_palermo_scale;
        a->sentry_info->sentry_diameter_km = sentry_diameter_km;
        a->sentry_info->sentry_impact_prob = copiar_texto(sentry_impact_prob);
    }

    a->asteroid_designation = copiar_texto(asteroid_designation);
    a->asteroid_fullname = copiar_texto(asteroid_fullname);
    a->close_approach_date = copiar_texto(close_approach_date);
    a->threat_category = copiar_texto(threat_category);
    a->panic_verdict = copiar_texto(panic_verdict);

    if(a->asteroid_designation == NULL || a->asteroid_fullname == NULL ||
       a->close_approach_date == NULL || a->threat_category == NULL ||
       a->panic_verdict == NULL ||
       (a->sentry_info != NULL && a->sentry_info->sentry_impact_prob == NULL)){
        nasa_apagar(a);
        return NULL;
    }

    a->year = year;
    a->month = month;
    a->distance_au = distance_au;
    a->velocity_km_s = velocity_km_s;
    a->absolute_magnitude = absolute_magnitude;
    a->days_until_approach = days_until_approach;
    a->is_past_event = is_past_event;
    a->is_future_event = is_future_event;
    a->risk_score = risk_score;
    a->panic_level = panic_level;
    a->on_sentry_list = on_sentry_list;

    //o ITEM generico passa a ser dono de "a"; se falhar, ainda somos nos;
    ITEM *item = item_criar(&ITEM_OPS_NASA, a);
    if(item == NULL){
        nasa_apagar(a);
    }
    return item;
}

//dados do asteroide dentro do ITEM; NULL se o ITEM nao for da NASA;
static const ITEM_NASA *nasa(const ITEM *item){
    if(item == NULL || item_get_ops(item) != &ITEM_OPS_NASA) return NULL;
    return (const ITEM_NASA*) item_get_dados(item);
}

/* --- GETTERS ESPECIFICOS (recebem o ITEM generico) --- */

const char *item_get_asteroid_designation(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    return a ? a->asteroid_designation : NULL;
}

const char *item_get_asteroid_fullname(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    return a ? a->asteroid_fullname : NULL;
}

const char *item_get_close_approach_date(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    return a ? a->close_approach_date : NULL;
}

const char *item_get_threat_category(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    return a ? a->threat_category : NULL;
}

const char *item_get_panic_verdict(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    return a ? a->panic_verdict : NULL;
}

unsigned int item_get_year(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    return a ? a->year : 0;
}

unsigned int item_get_month(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    return a ? a->month : 0;
}

double item_get_distance_au(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    return a ? a->distance_au : 0.0;
}

double item_get_velocity_km_s(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    return a ? a->velocity_km_s : 0.0;
}

float item_get_absolute_magnitude(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    return a ? a->absolute_magnitude : 0.0f;
}

int item_get_days_until_approach(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    return a ? a->days_until_approach : 0;
}

bool item_get_is_past_event(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    return a ? a->is_past_event : false;
}

bool item_get_is_future_event(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    return a ? a->is_future_event : false;
}

double item_get_risk_score(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    return a ? a->risk_score : 0.0;
}

unsigned int item_get_panic_level(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    return a ? a->panic_level : 0;
}

bool item_get_on_sentry_list(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    return a ? a->on_sentry_list : false;
}

const char *item_get_sentry_impact_prob(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    if (a == NULL || !a->on_sentry_list || a->sentry_info == NULL) {
        return NULL;
    }
    return a->sentry_info->sentry_impact_prob;
}

float item_get_sentry_torino_scale(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    if (a == NULL || !a->on_sentry_list || a->sentry_info == NULL) {
        return 0.0f;
    }
    return a->sentry_info->sentry_torino_scale;
}

float item_get_sentry_palermo_scale(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    if (a == NULL || !a->on_sentry_list || a->sentry_info == NULL) {
        return 0.0f;
    }
    return a->sentry_info->sentry_palermo_scale;
}

float item_get_sentry_diameter_km(const ITEM *item) {
    const ITEM_NASA *a = nasa(item);
    if (a == NULL || !a->on_sentry_list || a->sentry_info == NULL) {
        return 0.0f;
    }
    return a->sentry_info->sentry_diameter_km;
}

/* --- ACESSO A CAMPOS POR NOME ---
 * Unico lugar do projeto que associa o nome de uma coluna da NASA ao seu
 * tipo e ao valor correspondente. O resto do programa chega aqui pelas
 * ITEM_OPS_NASA (via item.c); para um novo campo, basta acrescenta-lo em
 * ID_CAMPO, em CAMPOS e no switch do seu tipo. */
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
    int casas;       //casas decimais usadas por nasa_formatar (so numericos);
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

static const char *texto_por_id(const ITEM_NASA *a, int id){
    switch (id) {
        case CP_ASTEROID_DESIGNATION: return a->asteroid_designation;
        case CP_ASTEROID_FULLNAME:    return a->asteroid_fullname;
        case CP_CLOSE_APPROACH_DATE:  return a->close_approach_date;
        case CP_THREAT_CATEGORY:      return a->threat_category;
        case CP_PANIC_VERDICT:        return a->panic_verdict;
        case CP_SENTRY_IMPACT_PROB:
            return (a->on_sentry_list && a->sentry_info) ? a->sentry_info->sentry_impact_prob : NULL;
        default:                      return NULL;
    }
}

static double num_por_id(const ITEM_NASA *a, int id){
    //campos Sentry valem 0 fora da lista Sentry, como nos getters especificos;
    const SentryInfo *s = (a->on_sentry_list && a->sentry_info) ? a->sentry_info : NULL;

    switch (id) {
        case CP_YEAR:                 return (double) a->year;
        case CP_MONTH:                return (double) a->month;
        case CP_DISTANCE_AU:          return a->distance_au;
        case CP_VELOCITY_KM_S:        return a->velocity_km_s;
        case CP_ABSOLUTE_MAGNITUDE:   return (double) a->absolute_magnitude;
        case CP_DAYS_UNTIL_APPROACH:  return (double) a->days_until_approach;
        case CP_RISK_SCORE:           return a->risk_score;
        case CP_PANIC_LEVEL:          return (double) a->panic_level;
        case CP_SENTRY_TORINO_SCALE:  return s ? (double) s->sentry_torino_scale : 0.0;
        case CP_SENTRY_PALERMO_SCALE: return s ? (double) s->sentry_palermo_scale : 0.0;
        case CP_SENTRY_DIAMETER_KM:   return s ? (double) s->sentry_diameter_km : 0.0;
        default:                      return 0.0;
    }
}

static bool bool_por_id(const ITEM_NASA *a, int id){
    switch (id) {
        case CP_IS_PAST_EVENT:   return a->is_past_event;
        case CP_IS_FUTURE_EVENT: return a->is_future_event;
        case CP_ON_SENTRY_LIST:  return a->on_sentry_list;
        default:                 return false;
    }
}

/* --- OPERACOES DO DATASET (chamadas por item.c) ---
 * item.c ja confere o tipo do campo antes de chamar get_texto/get_num/get_bool. */

static TIPO_CAMPO nasa_campo_tipo(const char *campo){
    int id = campo_id(campo);
    return id < 0 ? CAMPO_DESCONHECIDO : CAMPOS[id].tipo;
}

static const char *nasa_get_texto(const void *dados, const char *campo){
    return texto_por_id((const ITEM_NASA*) dados, campo_id(campo));
}

static double nasa_get_num(const void *dados, const char *campo){
    return num_por_id((const ITEM_NASA*) dados, campo_id(campo));
}

static bool nasa_get_bool(const void *dados, const char *campo){
    return bool_por_id((const ITEM_NASA*) dados, campo_id(campo));
}

//formato proprio da NASA: casas decimais por campo e "N/A" nos campos Sentry;
static void nasa_formatar(const void *dados, const char *campo, char *dest, size_t tam_dest){
    const ITEM_NASA *a = (const ITEM_NASA*) dados;
    int id = campo_id(campo);
    if (id < 0) {
        snprintf(dest, tam_dest, "DESCONHECIDO");
        return;
    }

    switch (CAMPOS[id].tipo) {
        case CAMPO_TEXTO: {
            const char *val = texto_por_id(a, id);
            snprintf(dest, tam_dest, "%s", val ? val : "N/A");
            break;
        }
        case CAMPO_BOOL:
            snprintf(dest, tam_dest, "%s", bool_por_id(a, id) ? "true" : "false");
            break;
        case CAMPO_NUMERO:
            if (CAMPOS[id].so_sentry && !a->on_sentry_list) {
                snprintf(dest, tam_dest, "N/A");
            } else {
                snprintf(dest, tam_dest, "%.*f", CAMPOS[id].casas, num_por_id(a, id));
            }
            break;
        default:
            snprintf(dest, tam_dest, "DESCONHECIDO");
            break;
    }
}

const ITEM_OPS ITEM_OPS_NASA = {
    "nasa",
    nasa_campo_tipo,
    nasa_get_texto,
    nasa_get_num,
    nasa_get_bool,
    nasa_formatar,
    nasa_apagar
};
