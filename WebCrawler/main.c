#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr ou inet_aton
#include "socketCrawler.h"
#include <unistd.h> //close

int main(int argc, char *argv[]){
    int sock_desc; //descritor do socket
    int *psock = &sock_desc;
    struct sockaddr_in servidor; //struct que montará o tipo de servidor
    struct sockaddr_in *pserv = &servidor;
    char *end = argv[1]; //endereço IP do site a ser visitado
    char *url = argv[2]; //url do site a ser visitado no formato www.site.com
    FILE *fp; //arquivo onde será armazenado a resposta do servidor
    fp = fopen("site.html", "w");


    criarSocket(psock);
    criarServidor(pserv, end);
    conectarServidor(sock_desc,servidor,url,fp);

    fclose(fp);
    close(sock_desc);

    return 0;
}
