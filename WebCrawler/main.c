#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include "socketCrawler.h"
#include "analizador.h"
#include <unistd.h> //close
#include <netdb.h> //struct addrinfo e função getaddrinfo
#define LEN_BUFFER 312

int main(int argc, char *argv[]){

    for(int i = 1 ; i<argc ; i++){
        pthread_t thread;
        pthread_create(&thread,NULL,percorrer_dominio,(void*)argv[i]);
        pthread_join(thread,NULL);
    }

    return 0;
}