#include <string.h>
#include "leitor.h"
#include "leitor_nasa.h"
#include "leitor_sti.h"

/* Registro dos leitores disponiveis. Para um dataset novo, basta incluir o
 * seu header e acrescentar o seu LEITOR_TIPO aqui; a main nao muda. */
static const LEITOR_TIPO *const LEITORES[] = {
    &LEITOR_NASA_TIPO,
    &LEITOR_STI_TIPO
};

#define QTD_LEITORES (sizeof(LEITORES) / sizeof(LEITORES[0]))

const LEITOR_TIPO *leitor_tipo(const char *nome) {
    if (!nome) return NULL;
    for (size_t i = 0; i < QTD_LEITORES; i++) {
        if (!strcmp(nome, LEITORES[i]->nome)) return LEITORES[i];
    }
    return NULL;
}

void leitor_listar(FILE *saida) {
    for (size_t i = 0; i < QTD_LEITORES; i++) {
        fprintf(saida, "%s%s", i ? ", " : "", LEITORES[i]->nome);
    }
}
