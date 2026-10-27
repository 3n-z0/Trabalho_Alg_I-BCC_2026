#include <stdlib.h>
#include "csv.h"

/* Codigo movido de leitor_nasa.c; antes a virgula e os 20 campos eram fixos. */

int csv_ler_registro(FILE *arquivo, char **linha) {
    *linha = NULL;
    size_t capacidade = 256, n = 0;
    char *buf = malloc(capacidade);
    if (!buf) return -1;
    bool aspas = false;
    int c;
    while ((c = fgetc(arquivo)) != EOF) {
        if (n + 1 >= capacidade) {
            if (capacidade > (size_t)-1 / 2) { free(buf); return -1; }
            capacidade *= 2;
            char *novo = realloc(buf, capacidade);
            if (!novo) { free(buf); return -1; }
            buf = novo;
        }
        if (c == 0) { free(buf); return -1; }
        buf[n++] = (char)c;
        if (c == '"') aspas = !aspas;
        if (c == '\n' && !aspas) break;
    }
    if (ferror(arquivo) || aspas) { free(buf); return -1; }
    if (!n) { free(buf); return 0; }
    buf[n] = '\0'; *linha = buf;
    return 1;
}

bool csv_separar(char *linha, char delimitador, char *campos[], size_t n_campos) {
    char *r = linha, *w = linha;  /* r le, w escreve: decodificar "" encurta o texto */
    size_t n = 0;
    for (;;) {
        if (n == n_campos) return false;  /* campos sobrando */
        campos[n++] = w;
        if (*r == '"') {
            r++;
            bool fechou = false;
            while (*r) {
                if (*r == '"') {
                    if (r[1] == '"') { *w++ = '"'; r += 2; }
                    else { r++; fechou = true; break; }
                } else *w++ = *r++;
            }
            if (!fechou) return false;
        } else {
            while (*r && *r != delimitador && *r != '\r' && *r != '\n') {
                if (*r == '"') return false;
                *w++ = *r++;
            }
        }
        char sep = *r;
        if (sep != delimitador && sep != '\0' && sep != '\r' && sep != '\n') return false;
        if (sep == delimitador) r++;
        else {
            if (*r == '\r') r++;
            if (*r == '\n') r++;
            if (*r) return false;
        }
        *w++ = '\0';
        if (sep != delimitador) return n == n_campos;  /* fim da linha: campos faltando? */
    }
}
