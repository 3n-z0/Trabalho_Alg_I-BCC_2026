#ifndef ITEM_H
    #define ITEM_H
    #include <stdbool.h>
    
    typedef struct item_ ITEM;

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
        float sentry_diameter_km
    );
    void item_apagar(ITEM **item);

//getters para strings;
    const char *item_get_asteroid_designation(const ITEM *item);
    const char *item_get_asteroid_fullname(const ITEM *item);
    const char *item_get_close_approach_date(const ITEM *item);
    const char *item_get_threat_category(const ITEM *item);
    const char *item_get_panic_verdict(const ITEM *item);
    
//getters para numéricos e booleanos;
    unsigned int item_get_year(const ITEM *item);
    unsigned int item_get_month(const ITEM *item);
    double item_get_distance_au(const ITEM *item);
    double item_get_velocity_km_s(const ITEM *item);
    float item_get_absolute_magnitude(const ITEM *item);
    int item_get_days_until_approach(const ITEM *item);
    bool item_get_is_past_event(const ITEM *item);
    bool item_get_is_future_event(const ITEM *item);
    double item_get_risk_score(const ITEM *item);
    unsigned int item_get_panic_level(const ITEM *item);
    bool item_get_on_sentry_list(const ITEM *item);
    
    const char *item_get_sentry_impact_prob(const ITEM *item);
    float item_get_sentry_torino_scale(const ITEM *item);
    float item_get_sentry_palermo_scale(const ITEM *item);
    float item_get_sentry_diameter_km(const ITEM *item);
#endif