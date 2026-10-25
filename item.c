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
    char sentry_impact_prob[21];
}SentryInfo;


struct item_
{
    //8 bytes;
    double risk_score; 
    double distance_au;
    double velocity_km_s;
    SentryInfo *sentry_info; //será NULL se on_sentry_info == false;

    //4 bytes;
    unsigned int year, month;
    unsigned int panic_level;
    int days_until_approach;
    float absolute_magnitude;
    
    //1 byte;
    bool on_sentry_list;
    bool is_past_event;
    bool is_future_event;
    char threat_category[10];
    char panic_verdict[35];
    char asteroid_designation[11];
    char asteroid_fullname[35];
    char close_approach_date[20];
    
};

ITEM *item_criar(
const char asteroid_designation[11],
const char asteroid_fullname[35],
const char close_approach_date[20],
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
const char threat_category[10],
const char panic_verdict[35],
bool on_sentry_list,
const char sentry_impact_prob[21],
float sentry_torino_scale,
float sentry_palermo_scale,
float sentry_diameter_km){
    ITEM *item = (ITEM*)malloc(sizeof(ITEM));
    if(item == NULL){
        return NULL;
    }

    SentryInfo *sentry_info = NULL;
    if(on_sentry_list){ //atribui características condicionadas a estar na sentry list;
        sentry_info = (SentryInfo*)malloc(sizeof(SentryInfo));
        if(sentry_info == NULL){
            free(item);
            return NULL;
        }
        sentry_info->sentry_torino_scale = sentry_torino_scale;
        sentry_info->sentry_palermo_scale = sentry_palermo_scale;
        sentry_info->sentry_diameter_km = sentry_diameter_km;

        strncpy(sentry_info->sentry_impact_prob, sentry_impact_prob, sizeof(sentry_info->sentry_impact_prob));
        sentry_info->sentry_impact_prob[sizeof(sentry_info->sentry_impact_prob) - 1] = '\0';
    }

    strncpy(item->asteroid_designation, asteroid_designation, sizeof(item->asteroid_designation));
    item->asteroid_designation[sizeof(item->asteroid_designation) - 1] = '\0';

    strncpy(item->asteroid_fullname, asteroid_fullname, sizeof(item->asteroid_fullname));
    item->asteroid_fullname[sizeof(item->asteroid_fullname) - 1] = '\0';

    strncpy(item->close_approach_date, close_approach_date, sizeof(item->close_approach_date));
    item->close_approach_date[sizeof(item->close_approach_date) - 1] = '\0';

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

    strncpy(item->threat_category, threat_category, sizeof(item->threat_category));
    item->threat_category[sizeof(item->threat_category) - 1] = '\0';

    strncpy(item->panic_verdict, panic_verdict, sizeof(item->panic_verdict));
    item->panic_verdict[sizeof(item->panic_verdict) - 1] = '\0';

    item->on_sentry_list = on_sentry_list;
    item->sentry_info = sentry_info;

    return item;
}

void item_apagar(ITEM **item){
    if(item == NULL || *item == NULL){
        return;
    }
    if((*item)->on_sentry_list){
        free((*item)->sentry_info);
        (*item)->sentry_info = NULL;
    }
    free(*item);
    *item = NULL;
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

void item_get_campo_str(const ITEM *item, const char *campo, char *dest, size_t tam_dest){
    if (item == NULL || campo == NULL || dest == NULL || tam_dest == 0) return;

    // --- CAMPOS DE TEXTO ---
    if (strcmp(campo, "threat_category") == 0) {
        const char *val = item_get_threat_category(item);
        snprintf(dest, tam_dest, "%s", val ? val : "N/A");
    }
    else if (strcmp(campo, "panic_verdict") == 0) {
        const char *val = item_get_panic_verdict(item);
        snprintf(dest, tam_dest, "%s", val ? val : "N/A");
    }
    else if (strcmp(campo, "asteroid_designation") == 0) {
        const char *val = item_get_asteroid_designation(item);
        snprintf(dest, tam_dest, "%s", val ? val : "N/A");
    }
    else if (strcmp(campo, "asteroid_fullname") == 0) {
        const char *val = item_get_asteroid_fullname(item);
        snprintf(dest, tam_dest, "%s", val ? val : "N/A");
    }
    else if (strcmp(campo, "close_approach_date") == 0) {
        const char *val = item_get_close_approach_date(item);
        snprintf(dest, tam_dest, "%s", val ? val : "N/A");
    }
    else if (strcmp(campo, "sentry_impact_prob") == 0) {
        const char *prob = item_get_sentry_impact_prob(item);
        snprintf(dest, tam_dest, "%s", prob ? prob : "N/A");
    }

    // --- CAMPOS INTEIROS E BOOLEANOS ---
    else if (strcmp(campo, "year") == 0) {
        snprintf(dest, tam_dest, "%u", item_get_year(item));
    }
    else if (strcmp(campo, "month") == 0) {
        snprintf(dest, tam_dest, "%u", item_get_month(item));
    }
    else if (strcmp(campo, "panic_level") == 0) {
        snprintf(dest, tam_dest, "%u", item_get_panic_level(item));
    }
    else if (strcmp(campo, "days_until_approach") == 0) {
        snprintf(dest, tam_dest, "%d", item_get_days_until_approach(item));
    }
    else if (strcmp(campo, "is_future_event") == 0) {
        snprintf(dest, tam_dest, "%s", item_get_is_future_event(item) ? "true" : "false");
    }
    else if (strcmp(campo, "is_past_event") == 0) {
        snprintf(dest, tam_dest, "%s", item_get_is_past_event(item) ? "true" : "false");
    }
    else if (strcmp(campo, "on_sentry_list") == 0) {
        snprintf(dest, tam_dest, "%s", item_get_on_sentry_list(item) ? "true" : "false");
    }

    // --- CAMPOS FLUTUANTES (DOUBLES E FLOATS) ---
    else if (strcmp(campo, "distance_au") == 0) {
        snprintf(dest, tam_dest, "%.4f", item_get_distance_au(item));
    }
    else if (strcmp(campo, "velocity_km_s") == 0) {
        snprintf(dest, tam_dest, "%.2f", item_get_velocity_km_s(item));
    }
    else if (strcmp(campo, "risk_score") == 0) {
        snprintf(dest, tam_dest, "%.4f", item_get_risk_score(item));
    }
    else if (strcmp(campo, "absolute_magnitude") == 0) {
        snprintf(dest, tam_dest, "%.2f", item_get_absolute_magnitude(item));
    }
    else if (strcmp(campo, "sentry_torino_scale") == 0) {
        if (!item_get_on_sentry_list(item)) {
            snprintf(dest, tam_dest, "N/A");
        } else {
            snprintf(dest, tam_dest, "%.1f", item_get_sentry_torino_scale(item));
        }
    }
    else if (strcmp(campo, "sentry_palermo_scale") == 0) {
        if (!item_get_on_sentry_list(item)) {
            snprintf(dest, tam_dest, "N/A");
        } else {
            snprintf(dest, tam_dest, "%.2f", item_get_sentry_palermo_scale(item));
        }
    }
    else if (strcmp(campo, "sentry_diameter_km") == 0) {
        if (!item_get_on_sentry_list(item)) {
            snprintf(dest, tam_dest, "N/A");
        } else {
            snprintf(dest, tam_dest, "%.3f", item_get_sentry_diameter_km(item));
        }
    }
    else {
        snprintf(dest, tam_dest, "DESCONHECIDO");
    }
}

double item_get_campo_num(const ITEM *item, const char *campo){
    if(campo == NULL) return 0.0;

    if (strcmp(campo, "distance_au") == 0) return item_get_distance_au(item);
    if (strcmp(campo, "velocity_km_s") == 0) return item_get_velocity_km_s(item);
    if (strcmp(campo, "risk_score") == 0) return item_get_risk_score(item);
    if (strcmp(campo, "year") == 0) return (double) item_get_year(item);
    if (strcmp(campo, "month") == 0) return (double) item_get_month(item);
    if (strcmp(campo, "absolute_magnitude") == 0) return (double) item_get_absolute_magnitude(item);
    if (strcmp(campo, "panic_level") == 0) return (double) item_get_panic_level(item);
    if (strcmp(campo, "days_until_approach") == 0) return (double) item_get_days_until_approach(item);


    if (strcmp(campo, "sentry_torino_scale") == 0) return (double) item_get_sentry_torino_scale(item);
    if (strcmp(campo, "sentry_palermo_scale") == 0) return (double) item_get_sentry_palermo_scale(item);
    if (strcmp(campo, "sentry_diameter_km") == 0) return (double) item_get_sentry_diameter_km(item);

    return 0.0;
}

bool item_campo_eh_numerico(const char *campo){
    static const char *const numericos[] = {
        "distance_au", "velocity_km_s", "risk_score", "year", "month",
        "absolute_magnitude", "panic_level", "days_until_approach",
        "sentry_torino_scale", "sentry_palermo_scale", "sentry_diameter_km"
    };
    if (campo == NULL) return false;

    for (size_t i = 0; i < sizeof(numericos) / sizeof(numericos[0]); i++) {
        if (strcmp(campo, numericos[i]) == 0) return true;
    }
    return false;
}
