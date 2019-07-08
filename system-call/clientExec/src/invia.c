#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>

#include "errExit.h"

struct myMSG {
    long mtype;
    char mtext[100];
};

int main (int argc, char *argv[]) {
    printf("\n***************Benvenuto nel programma INVIA!***************\n");

    key_t keyq = atoi(argv[3]);

    int msqid = msgget(keyq, IPC_CREAT | S_IRUSR | S_IWUSR);
    if(msqid == -1) {
        errExit("msgget fallita");
    }

    struct myMSG m;

    m.mtype = 1;
    memcpy(m.mtext, argv, sizeof m.mtext);

    size_t mSize = sizeof(struct myMSG)-sizeof(long);

    if(msgsnd(msqid, &m, mSize, 0) == -1) {
        errExit("msgsnd fallito");
    }
    printf("Argomenti inviati nella coda dei messaggi\n");

    return 0;
}
