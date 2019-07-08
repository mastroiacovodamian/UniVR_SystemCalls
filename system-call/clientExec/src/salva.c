#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <string.h>

int main (int argc, char *argv[]) {
    int i=0;

    printf("\n***************Benvenuto nel programma SALVA!***************\n");

    int fd_file;

    fd_file = open("salvataggio.txt", O_CREAT | O_RDWR, S_IRUSR |S_IWUSR);
    if(fd_file == -1) {
    	printf("open fallita");
    }

    for(i=3; i < argc; i++) {
    	ssize_t bW = write(fd_file, argv[i], strlen(argv[i]));
        if (bW != strlen(argv[i])) {
            printf("write fallita");
        }
    }
    printf("Argomenti salvati\n");
    close(fd_file);

    return 0;	
}
