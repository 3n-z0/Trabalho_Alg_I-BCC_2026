#ifndef TEMPO_H
    #define TEMPO_H

    /* Instante atual em milissegundos, de um relogio monotonico (tempo real
     * decorrido, nao tempo de CPU). So a diferenca entre duas leituras tem
     * significado: fim - inicio = duracao em ms. */
    double tempo_agora_ms(void);

#endif
