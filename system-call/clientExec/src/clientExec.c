#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include "errExit.h"
#include "request_response.h"
#include "memCondivisa.h"
#include "costanti.h"
#include "semafori.h"


int main (int argc, char *argv[]) {
    // argv[0]. Nome del programma
    // argv[1]. ID utente
    // argv[2]. chiave
    // argv[3]. argomenti 1 ... n
    if (argc < 4) {
        printf("Usa: %s ID_user Chiave Lista\n", argv[0]);
        exit(1);
    }

    int chiave;
    int i;
    char user[100];
    char nuovo[100];

    // Recupero chiave e utente
    chiave = atoi(argv[2]);
    memcpy(user, argv[1], sizeof user);

    // Preparazione semaforo
    key_t semkey = SEMKEY;
    int semid = semget(semkey, 1, S_IRUSR | S_IWUSR);

    // Preparazione memoria condivisa
    key_t shmKey = SHMKEY;
    size_t size = (sizeof(struct Nodo)) * SIZEMEM;
    printf("<ClientExec> Mi attacco alla memoria...\n");
    int shmid = alloc_shared_memory(shmKey, size);
    struct Nodo *n = (struct Nodo*)get_shared_memory(shmid, 0);

    // Verifico se la chiave NON è valida
    if(n->chiave == -1) {
        printf("La chiave non è utilizzabile per nessun servizio\n");
        printf("Il servizio inserito non è corretto o la chiave generata è già stata usata\n");
        n->pieno = 0;
        n->chiave = 0;
        memcpy(n->utente, nuovo, sizeof n->utente);
        return 0;
    }

    printf("<ClientExec> Verifico se l'utente e la chiave sono valide...!\n");
    semOp(semid, 0, -1);
    for(i=0; i<50; i++) {
        // Verifico se i dati coincidono
        if(n->pieno == 1 && n->chiave == chiave && strcmp(n->utente, user) == 0) {
            n->pieno = 0;
            n->timestamp = 0;
            memcpy(n->utente, nuovo, sizeof n->utente);
            // Determino il servizio scelto
            if(n->chiave > 0 && n->chiave <= 1000) {
                n->chiave = 0;
                semOp(semid, 0, 1);
                execv("stampa", argv);
            }
            else if(n->chiave >= 1001 && n->chiave <= 2000) {
                n->chiave = 0;
                semOp(semid, 0, 1);
                execv("salva", argv);
            }
            else if (n->chiave >= 2001 && n->chiave <= 3000){
                n->chiave = 0;
                semOp(semid, 0, 1);
                execv("invia", argv);
            }
        }
        n++;
    }
    semOp(semid, 0, 1);
    printf("La chiave non è utilizzabile per nessun servizio\n");
}