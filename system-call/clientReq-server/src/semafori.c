#include <sys/sem.h>
#include <sys/stat.h>
#include "semafori.h"
#include "errExit.h"

void semOp (int semid, unsigned short sem_num, short sem_op) {

    struct sembuf sop = {.sem_num = sem_num, .sem_op = sem_op, .sem_flg = 0};

    if (semop(semid, &sop, 1) == -1) {
        errExit("semop fallita");
    }
}

int create_sem_set(key_t semkey) {
    // Crea un set di semafori composto da un solo semaforo
    int semid = semget(semkey, 1, IPC_CREAT | S_IRUSR | S_IWUSR);
    if (semid == -1)
        errExit("semget failed");

    // Inizializzo a 1 il semaforo
    union semun arg;
    unsigned short values[] = {1};
    arg.array = values;

    if (semctl(semid, 0, SETALL, arg) == -1)
        errExit("semctl SETALL fallita");

    return semid;
}