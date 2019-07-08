#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/sem.h>

#include "errExit.h"
#include "request_response.h"
#include "keyManager.h"
#include "memCondivisa.h"
#include "costanti.h"
#include "semafori.h"

// Variabili globali
int shmid;
struct Nodo *n;

// Funzioni
void sigHandlerChild(int sig);

void keyManager(time_t timestamp) {

    int i;
    int c = 0;
    key_t shmKey = SHMKEY;
    time_t MAXTIME = TIME;
    size_t size = (sizeof(struct Nodo)) * SIZEMEM;

    printf("<KM> Sono KeyManager! -> %i\n", getpid());

    key_t semkey = SEMKEY;
    int semid = semget(semkey, 1, S_IRUSR | S_IWUSR);
    if(semid == -1) {
        errExit("semget fallita");
    }

    if (signal(SIGTERM, sigHandlerChild) == SIG_ERR) {
        errExit("signal handler fallita");
    }

    printf("<KM> Accedo in memoria...\n");
    shmid = alloc_shared_memory(shmKey, size);
    n = (struct Nodo*)get_shared_memory(shmid, 0);

    printf("<KM> Controllo le chiavi presenti in memoria ..\n");
    semOp(semid, 0, -1);
    for(i=0; i<50; i++) {
        if(n->pieno == 1) {
            c++;
            if((timestamp - n->timestamp > MAXTIME)) {
                n->pieno = 0;
                printf("<KM> Posizione %i-> Rimossa chiave presente in memoria da più di 5 min\n", i);
                c--;
            }
        }
        n++;
    }
    semOp(semid, 0, 1);
    printf("<KM> Presenti %i chiavi valide\n", c);
}

/* Signal Handler del processo
 * Termina quando riceve il SIGTERM
 */
void sigHandlerChild(int sig) {
    if (sig == SIGTERM) {
        printf("<KM> Ricevuto SIGTERM dal server!\n");
    }
    exit(0);
}