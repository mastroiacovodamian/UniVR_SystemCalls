#ifndef _ERREXIT_HH
#define _ERREXIT_HH

/* errExit è una funzione di supporto per stampare
 * il messaggio di errore dell'ultima chiamata di sistema fallita.
 * errExit termina anche il processo di chiamata.
 */
void errExit(const char *msg);

#endif
