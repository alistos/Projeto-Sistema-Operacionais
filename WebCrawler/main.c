#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include "socketCrawler.h"
#include "AnalizadorLexico/analizador.h"
#include <unistd.h> //close
#include <netdb.h> //struct addrinfo e função getaddrinfo
#define LEN_BUFFER 312

int main(int argc, char *argv[]){
    char *tipo_arquivo = argv[1];

    for(int i = 2 ; i<argc ; i++){
        char *dominio = argv[i];
        Arg_percorrer_dominio *args = start_arg_dominio(dominio,tipo_arquivo);
        pthread_t thread;
        pthread_create(&thread,NULL,percorrer_dominio,(void*)args);
        pthread_join(thread,NULL);
    }

    return 0;
}