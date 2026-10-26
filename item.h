#ifndef ITEM_H
    #define ITEM_H
    #include <stdbool.h>
    #include <stddef.h>
    
    typedef struct item_ ITEM;

    //os textos sao copiados no tamanho exato (sem limite); NULL vira "";
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
        float sentry_diameter_km
    );
    void item_apagar(ITEM **item);

//linha original do CSV (copiada), para gravar a saida no mesmo formato da entrada;
    bool item_set_linha_original(ITEM *item, const char *linha);
    const char *item_get_linha_original(const ITEM *item); //NULL se nao definida;

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

//acesso a campos pelo nome da coluna (ex.: "distance_au");
    typedef enum {
        CAMPO_DESCONHECIDO,
        CAMPO_TEXTO,
        CAMPO_NUMERO,
        CAMPO_BOOL
    } TIPO_CAMPO;

    //tipo do campo; CAMPO_DESCONHECIDO se o nome nao existir ou for NULL;
    TIPO_CAMPO item_campo_tipo(const char *campo);

    //cada getter so responde a campos do seu tipo: texto devolve NULL,
    //numero devolve 0.0 e bool devolve false para os demais ou para ausentes;
    const char *item_get_campo_texto(const ITEM *item, const char *campo);
    double item_get_campo_num(const ITEM *item, const char *campo);
    bool item_get_campo_bool(const ITEM *item, const char *campo);

    //valor de qualquer campo formatado como texto, para chaves de GROUP,
    //CROSS e ORDER textual; "N/A" se ausente, "DESCONHECIDO" se nao existir;
    void item_get_campo_str(const ITEM *item, const char *campo, char *dest, size_t tam_dest);
#endif