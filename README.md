# Trabalho_Alg_I-BCC_2026
Esse é o projeto da disciplina de Algoritmos e Estruturas de Dados do curso de Bacharelado em Ciência de Computação do ICMC-USP.

O programa carrega um CSV em uma estrutura de dados (a **base**), avalia uma consulta sobre cada registro e insere os aprovados em uma segunda estrutura (o **resultado**), que define a ordem da exportação. Os mesmos oito TADs atendem aos dois domínios:

- **Parte 1 – NASA:** aproximações de asteroides (`dados/asteroids.csv`, separador `,`, 20 colunas).
- **Parte 2 – STI:** movimentos migratórios do Sistema de Tráfego Internacional da Polícia Federal (`dados/sti.csv`, separador `;`, Windows-1252, CRLF, 5 colunas).

## Dados

| | NASA (`dados/asteroids.csv`) | STI (`dados/sti.csv`) |
|---|---|---|
| Registros (sem cabeçalho) | 89.227 | 11.683 (soma de `TOTAL`: 4.547.403) |
| Tamanho | 16.643.706 bytes | 368.170 bytes |
| Codificação / quebra de linha | ASCII (só bytes < 128) / CRLF | Windows-1252 / CRLF |
| SHA-256 | `be86bc6c2f1fb8670faa691d050c7900f67b2f851bd612f7ed5277cd5c3e54d6` | `1ecdb13cb54689da72d04277be61f92c0a2c6a7612ef0a8a6a52c82979cd7f0a` |
| Cobertura | aproximações de 2020-01-01 a 2100-12-30 | movimentos de janeiro de 2026 |
| Origem | arquivo do e-Disciplinas (`asteroid_dataset_20251019.csv`) — conferido em 05/11/2026 com nova cópia baixada da disciplina: SHA-256 idêntico | Dados abertos da Polícia Federal: arquivo `STI_2026_01.csv` do diretório de arquivos mensais <https://servicos.dpf.gov.br/dadosabertos/STI/> (recurso CSV do catálogo) |
| Catálogo | — | [STI - Sistema de tráfego internacional](https://dados.gov.br/dados/conjuntos-dados/sti---sistema-de-trafego-internacional), dados.gov.br (id `1aa1685d-833f-4c96-853e-2dccb1f9358d`; área técnica DIGED/CGTI/DTI/PF; licença CC0; atualização mensal) |
| Reserva na planilha da turma | — | reservado pelo grupo 39 em 26/10/2026 |

Na STI, `NACIONALIDADE` vem vazia em 19 registros; `TOTAL` não tem vazios.

## Compilação

```bash
make          # gera ./trabalho
make debug    # mesmo programa com AddressSanitizer/UBSan
make clean
```

Requer `gcc` com C99; o Makefile compila com `-Wall -Wextra -pedantic` sem avisos.

## Uso

```
./trabalho --reader LEITOR --input dados.csv --base ESTRUTURA
           --result ESTRUTURA --query consulta.txt --output resultado.csv
           [--top-k N] [--limit N]
```

| Opção | Significado |
|---|---|
| `--reader` | `nasa` ou `sti` |
| `--input` | CSV de entrada |
| `--base` / `--result` | `pilha`, `fila`, `lista_seq`, `lista_enc`, `lista_cabeca`, `lista_ord`, `lista_gen`, `lista_cruz` |
| `--query` | arquivo de consulta |
| `--output` | CSV de saída, no mesmo formato da entrada (cabeçalho e linhas originais) |
| `--top-k N` | exporta só os N primeiros do resultado (0 = todos); não altera `MATCHED_RECORDS` |
| `--limit N` | carrega só os N primeiros registros da entrada (0 = todos) |

Na saída padrão o programa informa `STATUS`, `INPUT_RECORDS`, `MATCHED_RECORDS`, `OUTPUT_RECORDS` e os tempos `LOAD_MS`, `SEARCH_MS`, `RESULT_INSERT_MS` e `WRITE_MS`.

### Consulta

Um comando por linha, campos separados por `|`; os nomes de campo são os do cabeçalho do CSV.

| Comando | Efeito |
|---|---|
| `FILTER\|campo\|op\|valor` | todos os filtros precisam ser satisfeitos; `op` ∈ `== != < <= > >=` (texto e booleano só `==` e `!=`) |
| `SIM\|campo\|alvo\|tolerância\|peso` | similaridade `max(0, 1 - \|x - alvo\| / tolerância)`, combinada por média ponderada |
| `ACCEPT\|limiar` | aceita só registros com similaridade ≥ limiar |
| `ORDER\|campo\|ASC` ou `ORDER\|campo\|DESC` | chave da `lista_ord` (`campo` pode ser `similarity`) |
| `GROUP\|campo1\|campo2` | chaves da `lista_gen` (grupos e subgrupos) |
| `CROSS\|campo1\|campo2` | linha e coluna da `lista_cruz` (exportada pelo percurso por linhas) |

`lista_ord`, `lista_gen` e `lista_cruz` exigem, respectivamente, `ORDER`, `GROUP` e `CROSS` na consulta. A `pilha` percorre em ordem inversa de inserção; `fila`, `lista_seq`, `lista_enc` e `lista_cabeca` percorrem na ordem de inserção. Como a busca percorre a base, uma base `pilha` também inverte a ordem em que os registros chegam ao resultado.

### Valores ausentes

Um campo vazio no CSV é **ausente**. O tratamento depende do tipo do campo, consultado pela interface genérica (`item_get_campo_*`, `item_campo_ausente`) que a consulta usa:

**Números e booleanos** — o ausente é distinto de `0` e de `false`:

- comparações em `FILTER` são falsas, exceto `!=`, que é verdadeira (ex.: `year` vazio não satisfaz `year == 0`);
- em `SIM` o campo contribui com similaridade 0;
- em `ORDER` numérico o registro vai para o fim;
- em `GROUP`/`CROSS` a chave é `N/A`.

**Textos** — o ausente é tratado como string vazia:

- em `FILTER`, `campo==` (valor vazio) aceita o registro ausente, e `!=` com qualquer texto não vazio também o aceita;
- em `GROUP`/`CROSS` a chave é vazia na NASA (separada de um texto literal `N/A`) e `N/A` na STI (`NACIONALIDADE`).

Na NASA, os campos `sentry_*` são ausentes quando `on_sentry_list` é falso. Os getters específicos de cada dataset (`item_get_year`, `item_get_is_future_event`...) continuam devolvendo o valor armazenado (0/`false` para vazios); a política acima vale para a interface genérica.

## Módulos

| Módulo | Responsabilidade |
|---|---|
| `main.c` | lê argumentos, monta base e resultado, executa a busca, exporta e mede tempos |
| `leitor.h/.c` | interface comum dos leitores (`--reader`) e registro dos disponíveis |
| `leitor_nasa.c`, `leitor_sti.c` | validam o cabeçalho e convertem cada linha em `ITEM` (únicos que conhecem o formato dos arquivos) |
| `csv.c` | leitura de registros (inclusive campos entre aspas com quebra de linha) e separação de campos, com delimitador e número de colunas por parâmetro |
| `item.h/.c` | `ITEM` genérico: dados do dataset, `ITEM_OPS` e linha original; acesso a campos pelo nome |
| `item_nasa.c`, `item_sti.c` | struct de cada dataset, tabela de campos e implementação de `ITEM_OPS` |
| `query.c` | lê e valida a consulta; avalia FILTER, SIM e ACCEPT sobre um `ITEM` |
| `busca.c` | percorre a base e envia os aprovados ao resultado |
| `estrutura.c` | despacha inserção, percurso e destruição para o TAD escolhido; calcula chaves de ORDER/GROUP/CROSS |
| `saida.c` | grava o CSV de saída com o cabeçalho e as linhas originais, respeitando `--top-k` |
| `tempo.c` | relógio monotônico em milissegundos |
| `pilha.c`, `fila.c`, `lista_seq.c`, `lista_enc.c`, `lista_cabeca.c` | TADs lineares |
| `lista_ord.c` | lista ordenada por chave numérica ou textual |
| `lista_gen.c` | lista generalizada: grupos por `campo1`, subgrupos por `campo2` |
| `lista_cruz.c` | lista cruzada: cada célula pertence a uma linha e a uma coluna; percursos pelos dois eixos e remoção individual |

O resultado guarda ponteiros emprestados para os `ITEM`s da base; somente a base os libera.

## Exemplos

Os seis exemplos abaixo foram executados com os arquivos de `dados/` e as consultas de `consultas/`. Os tempos (`*_MS`) foram omitidos porque variam a cada execução; das saídas são mostradas as primeiras linhas após o cabeçalho.

### Parte 1 – NASA

#### 1. Asteroides da lista Sentry agrupados por categoria e ano

```bash
./trabalho --reader nasa --input dados/asteroids.csv --base lista_seq --result lista_gen --query consultas/q1.txt --output resultado.csv
```

`consultas/q1.txt`:
```
FILTER|on_sentry_list|==|true
FILTER|distance_au|<=|0.08
SIM|velocity_km_s|15.0|10.0|1.0
SIM|risk_score|0.70|0.30|2.0
ACCEPT|0.60
ORDER|similarity|DESC
GROUP|threat_category|year
CROSS|threat_category|year
```

Saída padrão:
```
STATUS=OK
INPUT_RECORDS=89227
MATCHED_RECORDS=986
OUTPUT_RECORDS=986
```

`resultado.csv`:
```
2020 BY1,       (2020 BY1),2020-01-16 23:46:00,2020,1,0.0107597078807768,16.8368873335429,26.4,-2103,True,False,0.5581488175745649,6,CONCERN,Time to learn survival skills?,True,7.90754e-06,0.0,-5.31,0.018
2020 CA,       (2020 CA),2020-02-02 15:49:00,2020,2,0.0014655652786794,13.2600419467177,29.6,-2086,True,False,0.5598123478332484,6,CONCERN,Time to learn survival skills?,True,1.13726e-06,0.0,-8.38,0.0041
```

#### 2. Aproximações de até 0,01 UA em MONITOR, da mais próxima à mais distante

```bash
./trabalho --reader nasa --input dados/asteroids.csv --base lista_enc --result lista_ord --query consultas/q2.txt --output resultado.csv
```

`consultas/q2.txt`:
```
FILTER|distance_au|<=|0.01
FILTER|threat_category|==|MONITOR
ORDER|distance_au|ASC
GROUP|threat_category|year
CROSS|threat_category|year
```

Saída padrão:
```
STATUS=OK
INPUT_RECORDS=89227
MATCHED_RECORDS=1387
OUTPUT_RECORDS=1387
```

`resultado.csv` (campos Sentry vazios: asteroide fora da lista Sentry):
```
2023 BU,       (2023 BU),2023-01-27 00:29:00,2023,1,6.66251002445381e-05,9.2672451513957,29.69,-997,True,False,0.44371723534831636,4,MONITOR,"Worth a tweet, not worth a bunker.",False,,,,
2022 KP6,       (2022 KP6),2022-05-25 15:05:00,2022,5,0.0001164888806837,9.882333671434,29.41,-1243,True,False,0.4465279657610484,4,MONITOR,"Worth a tweet, not worth a bunker.",False,,,,
```

#### 3. Similaridade de magnitude na lista Sentry, cruzando categoria e nível de pânico (top-5)

```bash
./trabalho --reader nasa --input dados/asteroids.csv --base lista_cabeca --result lista_cruz --query consultas/q3.txt --output resultado.csv --top-k 5
```

`consultas/q3.txt`:
```
FILTER|on_sentry_list|==|true
SIM|absolute_magnitude|22.0|5.0|1.0
ACCEPT|0.5
ORDER|threat_category|ASC
GROUP|year|month
CROSS|threat_category|panic_level
```

Saída padrão (`--top-k` limita só a exportação):
```
STATUS=OK
INPUT_RECORDS=89227
MATCHED_RECORDS=191
OUTPUT_RECORDS=5
```

`resultado.csv`:
```
2020 AO1,       (2020 AO1),2020-01-12 19:18:00,2020,1,0.02184482086146,17.7016569143278,23.7,-2107,True,False,0.5400704121577328,5,CONCERN,Time to learn survival skills?,True,1.474e-07,0.0,-5.73,0.062
2020 DF3,       (2020 DF3),2020-02-21 18:57:00,2020,2,0.0217936707296065,26.305738536251,24.2,-2067,True,False,0.5808852604776058,6,CONCERN,Time to learn survival skills?,True,2.808686e-06,0.0,-4.95,0.0497
```

### Parte 2 – STI

O arquivo da STI está em Windows-1252 (não Latin-1 estrito: a linha 6.054, `CÔTE D’IVOIRE`, usa o byte `0x92` como apóstrofo). As linhas abaixo foram convertidas para UTF-8 apenas para exibição; a saída conserva os bytes originais.

#### 4. Entradas cruzadas por UF e classificação

```bash
./trabalho --reader sti --input dados/sti.csv --base lista_seq --result lista_cruz --query consultas/sti_q1.txt --output resultado_sti.csv
```

`consultas/sti_q1.txt`:
```
FILTER|TIPO|==|ENTRADA
ORDER|TOTAL|DESC
GROUP|UF_ATENDIMENTO|CLASSIFICACAO
CROSS|UF_ATENDIMENTO|CLASSIFICACAO
```

Saída padrão:
```
STATUS=OK
INPUT_RECORDS=11683
MATCHED_RECORDS=5916
OUTPUT_RECORDS=5916
```

`resultado_sti.csv`:
```
AC;ENTRADA;PERMANENTE;ARGENTINA;1
AC;ENTRADA;PERMANENTE;BOLÍVIA;65
AC;ENTRADA;PERMANENTE;CHILE;3
```

#### 5. Visitantes com TOTAL próximo de 1.000 movimentos, por similaridade

```bash
./trabalho --reader sti --input dados/sti.csv --base fila --result lista_ord --query consultas/sti_q3.txt --output resultado_sti.csv
```

`consultas/sti_q3.txt`:
```
FILTER|CLASSIFICACAO|==|VISITANTE
SIM|TOTAL|1000|500|1.0
ACCEPT|0.5
ORDER|similarity|DESC
GROUP|UF_ATENDIMENTO|TIPO
CROSS|NACIONALIDADE|UF_ATENDIMENTO
```

Saída padrão:
```
STATUS=OK
INPUT_RECORDS=11683
MATCHED_RECORDS=61
OUTPUT_RECORDS=61
```

`resultado_sti.csv`:
```
AM;SAIDA;VISITANTE;ESTADOS UNIDOS;1002
RS;SAIDA;VISITANTE;CHILE;1008
SP;ENTRADA;VISITANTE;COLÔMBIA;992
```

#### 6. Entradas permanentes de venezuelanos agrupadas por UF

```bash
./trabalho --reader sti --input dados/sti.csv --base pilha --result lista_gen --query consultas/sti_q4.txt --output resultado_sti.csv
```

`consultas/sti_q4.txt`:
```
FILTER|NACIONALIDADE|==|VENEZUELA
FILTER|CLASSIFICACAO|==|PERMANENTE
FILTER|TIPO|==|ENTRADA
ORDER|TOTAL|DESC
GROUP|UF_ATENDIMENTO|TIPO
CROSS|UF_ATENDIMENTO|TIPO
```

Saída padrão:
```
STATUS=OK
INPUT_RECORDS=11683
MATCHED_RECORDS=16
OUTPUT_RECORDS=16
```

`resultado_sti.csv` (a base `pilha` entrega os registros do último para o primeiro, e os grupos seguem essa ordem de chegada):
```
SP;ENTRADA;PERMANENTE;VENEZUELA;427
SC;ENTRADA;PERMANENTE;VENEZUELA;37
RS;ENTRADA;PERMANENTE;VENEZUELA;35
```
