CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -pedantic -O2
LDLIBS  = -lm

EXEC    = trabalho
FONTES  = main.c busca.c estrutura.c leitor_nasa.c query.c item.c tempo.c \
          pilha.c fila.c lista_seq.c lista_enc.c lista_cabeca.c \
          lista_ord.c lista_gen.c lista_cruz.c
OBJETOS = $(FONTES:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJETOS)
	$(CC) $(CFLAGS) -o $@ $(OBJETOS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Mesmo programa com AddressSanitizer/UBSan, para checar memoria nos testes.
debug: CFLAGS = -std=c99 -Wall -Wextra -pedantic -g -O0 -fsanitize=address,undefined
debug: clean $(EXEC)

clean:
	rm -f $(OBJETOS) $(EXEC)

.PHONY: all debug clean
