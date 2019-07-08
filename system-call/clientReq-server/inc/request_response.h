#ifndef _REQUEST_RESPONSE_HH
#define _REQUEST_RESPONSE_HH

#include <sys/types.h>

struct Request {           /* Request (client --> server) */
    pid_t cPid;            /* PID del Client              */
    char id[100];          /* ID dell'utente              */
    char servizio[100];    /* Servizio scelto             */
};

struct Response {          /* Response (server --> client) */
    int chiave;            /* Chiave del servizio          */
};

#endif