#ifndef _SHARED_MEMORY_HH
#define _SHARED_MEMORY_HH

#include <stdlib.h>

// Struttura che viene caricata nella memoria condivisa
struct Nodo {
    char utente[100];
    int chiave;
    time_t timestamp;
    int pieno;
};

/* Il metodo alloc crea, se non esiste, un segmento di memoria condiviso
 * con dimensioni size e chiave shmkey
 */
int alloc_shared_memory(key_t shmKey, size_t size);

/* Il metodo get collega un segmento di memoria condivisa nello spazio
 * degli indirizzi logici del processo chiamante
 */
void *get_shared_memory(int shmid, int shmflg);

/* Il metodo free stacca un segmento di memoria condivisa dallo spazio degli
 * indirizzi logici del processo chiamante
 */
void free_shared_memory(void *ptr_sh);

/* Il remove rimuove un segmento di memoria condivisa.
 * Se non riesce, termina il processo chiamante
 */
void remove_shared_memory(int shmid);

#endif
