#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>

#include "errExit.h"
#include "request_response.h"
#include "keyManager.h"
#include "memCondivisa.h"
#include "semafori.h"
#include "costanti.h"

// Funzioni
void quit(int sig);
int sendResponse(struct Request *request);
int create_sem_set(key_t semkey);

//Variabili globali
int shmid;
int semid;
int serverFIFO;
int serverFIFO_extra;
int chiaviusate[300];
pid_t figlio;
struct Nodo *n;
struct Request request;

// Percorsi FIFO
char *path2ServerFIFO = "/tmp/fifo_server";
char *baseClientFIFO = "/tmp/fifo_client";

int main (int argc, char *argv[]) {

    printf("Processo Server Avviato! PID -> %i\n", getpid());

    // Dichiaro il set dei segnali del processo
    sigset_t mySet;

    // Inizializzo il set per contenere tutti i segnali
    sigfillset(&mySet);

    // Rimuovo SIGTERM dal set
    sigdelset(&mySet, SIGTERM);

    // Blocco tutti i segnali del set, ovvero tutti tranne SIGTERM
    sigprocmask(SIG_SETMASK, &mySet, NULL);

    // Setto la signal handler (funzione quit) per il segnale SIGTERM
    if (signal(SIGTERM, quit) == SIG_ERR) {
        errExit("signal handler fallita");
    }

    // Server crea la FIFO
    printf("<Server> Creazione FIFO...\n");
    if (mkfifo(path2ServerFIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1) {
        errExit("mkfifo failed");
    }

    printf("<Server> FIFO %s creata!\n", path2ServerFIFO);

    // Attendo il client in sola lettura
    // La open blocca il server finchè il client non apre la stessa FIFO in sola scrittura
    printf("<Server> Attendo il client...\n");
    serverFIFO = open(path2ServerFIFO, O_RDONLY);
    if (serverFIFO == -1) {
        errExit("open fallita");
    }

    // Apro un descrittore in più, in modo che il server non veda la fine del file
    serverFIFO_extra = open(path2ServerFIFO, O_WRONLY);
    if (serverFIFO_extra == -1)
        errExit("open fallita");

    // Definizione variabili
    int bR = -1;
    int c = 0;
    key_t semkey = SEMKEY;

    // Creo set semafori
    semid = create_sem_set(semkey);

    // Preparazione memoria condivisa
    key_t shmKey = SHMKEY;
    size_t size = (sizeof(struct Nodo)) * SIZEMEM;
    shmid = alloc_shared_memory(shmKey, size);
    n = (struct Nodo*)get_shared_memory(shmid, 0);

    figlio = fork();
    switch (figlio) {
        case -1:
            errExit("fork fallita");
        case 0: {
            while(1) {
                sleep(30);
                keyManager(time(NULL));
            }
        }
        default: {
            do {
                printf("<Server> Attendo una richiesta...\n");

                // Leggo la Request dalla FIFO
                bR = read(serverFIFO, &request, sizeof(struct Request));

                // Controllo il numero di bytes letti dalla FIFO
                if (bR == -1) {
                    printf("<Server> Sembra che la FIFO sia corrotta\n");
                }
                else if (bR != sizeof(struct Request)) {
                    printf("<Server> Non ho ricevuto una richiesta valida\n");
                }
                else {
                    int i = 0;

                    // Generazione chiave e invio al client
                    c = sendResponse(&request);

                    semOp(semid, 0, -1);
                    // Salvo in memoria condivisa la richiesta ricevuta
                    for (i=0; i<50; i++) {
                        if (n->pieno == 0) {
                            printf("<Server> Sto caricando in memoria condivisa...\n");
                            memcpy(n->utente, request.id, sizeof n->utente);
                            n->chiave = c;
                            n->timestamp = time(NULL);
                            n->pieno = 1;
                            semOp(semid, 0, 1);
                            break;
                        }
                        n++;
                    }
                }
            }
            while (bR != -1);
        }
    }
    // Se la FIFO è corrotta, eseguo quit per chiudere FIFO, SHM e SEM
    quit(0);
}

/*  La funzione quit chiude le FIFO, SHM e SEM
 *  Inoltre, invia SIGTERM al figlio
 */
void quit(int sig) {
    if (sig == SIGTERM) {
        printf("<Server> Ricevuto SIGTERM!\n");
    }
    if(kill(figlio, SIGTERM) == -1) {
        printf("kill fallita");
    }
    // Chiudo le FIFO
    if (serverFIFO != 0 && close(serverFIFO) == -1) {
        errExit("close fallita");
    }
    if (serverFIFO_extra != 0 && close(serverFIFO_extra) == -1) {
        errExit("close fallita");
    }
    // Rimuovo le FIFO
    if (unlink(path2ServerFIFO) != 0) {
        //errExit("unlink failed");
    }

    // Rimuovo set di semafori
    if (semctl(semid, 0 /*ignored*/, IPC_RMID, NULL) == -1) {
        //errExit("semctl IPC_RMID failed");
    }

    // Rimuovo la memoria condivisa
    remove_shared_memory(shmid);

    // Termino il processo
    _exit(0);
}

/*  La funzione sendResponse genera la chiave e
 *  Invia la chiave al Client
 */
int sendResponse(struct Request *request) {

    int tmp = 4;
    int j = 0;
    struct Response response;

    // Preparo per la generazione random della chiave
    time_t t;
    srand((unsigned int) time(&t));

    // Costruisco il percorso della FIFO del Client
    char path2ClientFIFO[25];
    sprintf(path2ClientFIFO, "%s%d", baseClientFIFO, request->cPid);

    printf("<Server> Apertura FIFO %s...\n", path2ClientFIFO);
    // Apertura della FIFO del Client in sola scrittura
    int clientFIFO = open(path2ClientFIFO, O_WRONLY);
    if (clientFIFO == -1) {
        printf("open fallita");
        return 0;
    }

    // Verifico il servizio scelto dall'utente
    if (strcmp(request->servizio, "stampa") == 0) {
        tmp = 1;
    } else if (strcmp(request->servizio, "salva") == 0) {
        tmp = 2;
    } else if (strcmp(request->servizio, "invia") == 0) {
        tmp = 3;
    } else {
        tmp = 4;
    }

    // Genero la chiave a seconda del servizio scelto
    switch (tmp) {
        // Da 1 a 1000
        case 1:
            response.chiave = 1 + rand() % 1001;
            break;
            // Da 1001 a 2000
        case 2:
            response.chiave = 1001 + rand() % 1000;
            break;
            // Da 2001 a 3000
        case 3:
            response.chiave = 2001 + rand() % 1000;
            break;
            // In caso di servizio non valido
        case 4:
            response.chiave = -1;
    }

    // Verifico se è già stata usata, in questo caso la rendo non utilizzabile
    for (j=0; j<500; j++) {
        if(response.chiave == chiaviusate[j]) {
            response.chiave = -1;
        }
    }

    // La inserisco fra le chiavi usate
    for (j=0; j<500; j++) {
        if(chiaviusate[j] == 0) {
            chiaviusate[j] = response.chiave;
            break;
        }
    }

    printf("<Server> Invio chiave al Client...\n");
    // Scrivo la risposta nella FIFO aperta
    if (write(clientFIFO, &response, sizeof(struct Response)) != sizeof(struct Response)) {
        printf("<Server> write fallita");
    }

    // Chiudo la FIFO
    if (close(clientFIFO) != 0) {
        printf("<Server> close fallita");
        return 0;
    }
    return response.chiave;
}