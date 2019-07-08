#ifndef _SEMAPHORE_HH
#define _SEMAPHORE_HH

// Definizione della union semun
union semun {
    int val;
    struct semid_ds * buf;
    unsigned short * array;
};

/* semOp è una funzione di supporto per manipolare il valore di un semaforo
 * Semid è un identificatore di set di semafori, sem_num è l'indice di un semaforo nell'insieme
 * sem_op è l'operazione eseguita su sem_num
 */
void semOp (int semid, unsigned short sem_num, short sem_op);

/* create_sem_set serve a creare un set di semafori
 * Inoltre permette di inizializzarli
 */
int create_sem_set(key_t semkey);

#endif
