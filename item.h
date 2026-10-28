#ifndef ITEM_H
    #define ITEM_H
    #include <stdbool.h>
    #include <stddef.h>

    /* ITEM generico: um registro de qualquer dataset. Guarda os dados
     * especificos (struct do dataset, opaca aqui), as funcoes que sabem
     * le-los (ITEM_OPS) e a linha original do CSV. Estruturas, query,
     * busca e estrutura.c so usam esta interface. */
    typedef struct item_ ITEM;


    //constantes de tipo de campo;
    typedef enum {
        CAMPO_DESCONHECIDO,
        CAMPO_TEXTO,
        CAMPO_NUMERO,
        CAMPO_BOOL
    } TIPO_CAMPO;

    /* Operacoes que cada dataset implementa (item_nasa.c, item_sti.c...).
     * "dados" e a struct propria do dataset, criada pelo seu leitor. */
    typedef struct {
        const char *nome; //ex.: "nasa", "sti";
        TIPO_CAMPO (*campo_tipo)(const char *campo);
        const char *(*get_texto)(const void *dados, const char *campo); //NULL se ausente;
        double (*get_num)(const void *dados, const char *campo);       //NaN se ausente;
        bool (*get_bool)(const void *dados, const char *campo);
        //opcional (pode ser NULL): formatacao propria em texto (casas decimais, "N/A"...);
        void (*formatar)(const void *dados, const char *campo, char *dest, size_t tam);
        void (*apagar_dados)(void *dados);
    } ITEM_OPS;

    //dataset da execucao; a main define a partir de --reader, antes de query_criar;
    void item_definir_dataset(const ITEM_OPS *ops);
    const ITEM_OPS *item_dataset(void);

    //envolve os dados de um registro; o ITEM passa a ser dono de "dados";
    ITEM *item_criar(const ITEM_OPS *ops, void *dados);
    void item_apagar(ITEM **item);

    //acesso a struct do dataset, para os getters especificos (ex.: item_nasa.c);
    const void *item_get_dados(const ITEM *item);
    const ITEM_OPS *item_get_ops(const ITEM *item);

//linha original do CSV (copiada), para gravar a saida no mesmo formato da entrada;
    bool item_set_linha_original(ITEM *item, const char *linha);
    const char *item_get_linha_original(const ITEM *item); //NULL se nao definida;

//acesso a campos pelo nome da coluna (ex.: "distance_au", "TOTAL");
    //tipo no dataset da execucao; CAMPO_DESCONHECIDO se nao existir ou sem dataset;
    TIPO_CAMPO item_campo_tipo(const char *campo);

    //cada getter so responde a campos do seu tipo: texto devolve NULL,
    //numero devolve 0.0 e bool devolve false para os demais;
    const char *item_get_campo_texto(const ITEM *item, const char *campo);
    double item_get_campo_num(const ITEM *item, const char *campo);
    bool item_get_campo_bool(const ITEM *item, const char *campo);

    //valor de qualquer campo formatado como texto, para chaves de GROUP,
    //CROSS e ORDER textual; "N/A" se ausente, "DESCONHECIDO" se nao existir;
    void item_get_campo_str(const ITEM *item, const char *campo, char *dest, size_t tam_dest);
#endif
