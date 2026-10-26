/* clock_gettime e CLOCK_MONOTONIC sao POSIX, nao C99: com -std=c99 o
 * <time.h> so os declara se _POSIX_C_SOURCE for definido antes dele. */
#define _POSIX_C_SOURCE 199309L

#include <time.h>
#include "tempo.h"

double tempo_agora_ms(void){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double) t.tv_sec * 1000.0 + (double) t.tv_nsec / 1e6;
}
