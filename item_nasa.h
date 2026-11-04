#ifndef ITEM_NASA_H
    #define ITEM_NASA_H
    #include <stdbool.h>
    #include "item.h"

    /* Parte especifica do dataset de asteroides (Parte 1). Junto com
     * leitor_nasa, e o unico modulo que conhece as colunas da NASA; o resto
     * do programa acessa os campos pela interface generica de item.h. */

    //operacoes do dataset, para item_definir_dataset() e item_criar();
    extern const ITEM_OPS ITEM_OPS_NASA;

    //cria um ITEM da NASA; os textos sao copiados no tamanho exato (NULL vira "");
    ITEM *item_nasa_criar(
        const char *asteroid_designation,
        const char *asteroid_fullname,
        const char *close_approach_date,
        unsigned int year,
        unsigned int month,
        double distance_au,
        double velocity_km_s,
        double absolute_magnitude,
        int days_until_approach,
        bool is_past_event,
        bool is_future_event,
        double risk_score,
        unsigned int panic_level,
        const char *threat_category,
        const char *panic_verdict,
        bool on_sentry_list,
        double sentry_impact_prob,
        double sentry_torino_scale,
        double sentry_palermo_scale,
        double sentry_diameter_km
    );

    //marca um campo como vazio no CSV (ausente); usado pelo leitor logo apos
    //item_nasa_criar. Numeros ausentes passam a valer NaN e booleanos ausentes
    //nao satisfazem == em FILTER. Os getters especificos abaixo nao mudam;
    void item_nasa_marcar_ausente(ITEM *item, const char *campo);

//getters especificos; devolvem NULL/0 se o ITEM for NULL ou nao for da NASA;
    const char *item_get_asteroid_designation(const ITEM *item);
    const char *item_get_asteroid_fullname(const ITEM *item);
    const char *item_get_close_approach_date(const ITEM *item);
    const char *item_get_threat_category(const ITEM *item);
    const char *item_get_panic_verdict(const ITEM *item);

    unsigned int item_get_year(const ITEM *item);
    unsigned int item_get_month(const ITEM *item);
    double item_get_distance_au(const ITEM *item);
    double item_get_velocity_km_s(const ITEM *item);
    double item_get_absolute_magnitude(const ITEM *item);
    int item_get_days_until_approach(const ITEM *item);
    bool item_get_is_past_event(const ITEM *item);
    bool item_get_is_future_event(const ITEM *item);
    double item_get_risk_score(const ITEM *item);
    unsigned int item_get_panic_level(const ITEM *item);
    bool item_get_on_sentry_list(const ITEM *item);

    //campos Sentry: NaN se o asteroide nao estiver na lista Sentry (ausente);
    double item_get_sentry_impact_prob(const ITEM *item);
    double item_get_sentry_torino_scale(const ITEM *item);
    double item_get_sentry_palermo_scale(const ITEM *item);
    double item_get_sentry_diameter_km(const ITEM *item);
#endif
