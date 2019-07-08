#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include "request_response.h"
#include "errExit.h"
#include "costanti.h"

// Percorsi FIFO
char *path2ServerFIFO = "/tmp/fifo_server";
char *baseClientFIFO = "/tmp/fifo_client";

int main (int argc, char *argv[]) {

    char scelta[100];
    char id[100];
    int i = 0;

    printf("Benvenuto Utente! I servizi di sistema sono:\n Stampa\n Salva\n Invia\n");
    printf("Indica il servizio che vuoi utilizzare:\t");

    scanf("%s", scelta);

    while(scelta[i]) {
        scelta[i] = tolower(scelta[i]);
        i++;
    }

    printf("Inserire l'identificativo dell'utente\n");
    scanf("%s", id);

    // Client crea la fifo
    char path2ClientFIFO[25];
    sprintf(path2ClientFIFO, "%s%d", baseClientFIFO, getpid());

    printf("<Client> Sto creando la FIFO...\n");
    if (mkfifo(path2ClientFIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1) {
        errExit("mkfifo fallita");
    }
    printf("<Client> FIFO %s creata!\n", path2ClientFIFO);

    // Client apre la FIFO del server per poi inviare la Request
    printf("<Client> Apertura FIFO %s...\n", path2ServerFIFO);
    int serverFIFO = open(path2ServerFIFO, O_WRONLY);
    if (serverFIFO == -1) {
        errExit("open fallita");
    }

    // Preparo la richiesta
    struct Request request;
    request.cPid = getpid();
    memcpy(request.id, id, sizeof request.id);
    memcpy(request.servizio, scelta, sizeof request.servizio);

    // Client invia la richiesta attraverso la FIFO server
    if (write(serverFIFO, &request, sizeof(struct Request)) != sizeof(struct Request)) {
        errExit("write fallita");
    }

    // Client apre la FIFO per leggere la risposta
    int clientFIFO = open(path2ClientFIFO, O_RDONLY);
    if (clientFIFO == -1) {
        errExit("open fallita");
    }

    // Client legge la risposta del server
    struct Response response;
    if (read(clientFIFO, &response, sizeof(struct Response)) != sizeof(struct Response)) {
        errExit("read fallita");
    }

    // Client stampa su terminale il risultato
    printf("<Client>\nCodice id: %s\nServizio: %s\nChiave rilasciata: %i\n", id, scelta, response.chiave);

    // Client chiude le fifo
    if (close(serverFIFO) != 0 || close(clientFIFO) != 0) {
        errExit("close fallita");
    }

    // Client rimuove le FIFO dal file system
    if (unlink(path2ClientFIFO) != 0) {
        errExit("unlink fallita");
    }
}