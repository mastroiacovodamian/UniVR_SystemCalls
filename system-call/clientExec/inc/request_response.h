#ifndef _REQUEST_RESPONSE_HH
#define _REQUEST_RESPONSE_HH

#include <sys/types.h>

struct Request {        /* Request (client --> server) */
    pid_t cPid;         /* PID of client               */
    char *id;           /* ID of client               */
    char *servizio;     /* Servizio scelto             */
    int scelta;
};

struct Response {  /* Response (server --> client) */
    int chiave;    /* Chiave del servizio             */
};

#endif