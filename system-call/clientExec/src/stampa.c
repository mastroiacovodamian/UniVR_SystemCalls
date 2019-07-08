#include <stdlib.h>
#include <stdio.h>

int main (int argc, char *argv[]) {
    int i;

    printf("\n***************Benvenuto nel programma di STAMPA!***************\n");
    printf("La lista degli argomenti è:\n");

    for(i=3; i<argc; i++) {
    	printf("%s ", argv[i]);
    }
    printf("\n");

    return 0;
}
