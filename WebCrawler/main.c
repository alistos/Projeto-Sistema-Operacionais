#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include "socketCrawler.c"
#include <unistd.h> //close
#include <netdb.h> //struct addrinfo e função getaddrinfo

int main(int argc, char *argv[]){
    int sock_desc; //descritor do socket
    int *psock = &sock_desc;
    struct addrinfo hints, *res;
    struct addrinfo **pres = &res; 		
    char *end = argv[1]; //endereço do site a ser visitado
    FILE *fp; //arquivo onde será armazenado a resposta do servidor
    fp = fopen("site.html", "w");
    
    criarServidor(hints, pres, end);
    criarSocket(psock, res);    
    conectarServidor(sock_desc,res,end,fp);

    fclose(fp);
    close(sock_desc);

    return 0;
}
