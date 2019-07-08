#include <sys/shm.h>
#include <sys/stat.h>

#include "errExit.h"
#include "memCondivisa.h"

int alloc_shared_memory(key_t shmKey, size_t size) {
    // Prende, o crea, un segmento di memoria condivisa
    int shmid = shmget(shmKey, size, IPC_CREAT | S_IRUSR | S_IWUSR);
    if (shmid == -1)
        errExit("shmget failed");

    return shmid;
}

void *get_shared_memory(int shmid, int shmflg) {
    // "Attacca" la memoria condivisa
    void *ptr_sh = shmat(shmid, NULL, shmflg);
    if (ptr_sh == (void *)-1) {
        errExit("shmat fallita");
    }
    return ptr_sh;
}

void free_shared_memory(void *ptr_sh) {
    // "Stacca" la memoria condivisa
    if (shmdt(ptr_sh) == -1) {
        errExit("shmdt fallita");
    }
}

void remove_shared_memory(int shmid) {
    // Elimina segmento di memoria condivisa
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        errExit("shmctl fallita");
    }
}
