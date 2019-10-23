#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include "socketCrawler.h"
#include "analizador.h"
#include <unistd.h> //close
#include <netdb.h> //struct addrinfo e função getaddrinfo

int main(int argc, char *argv[]){
    int sock_desc; //descritor do socket
    int *psock = &sock_desc;
    struct addrinfo hints, *res;
    struct addrinfo **pres = &res; 		
    char *end = argv[1]; //endereço do site a ser visitado
    FILE *fp; //arquivo onde será armazenado a resposta do servidor
    char* nome_arquivo_saida = "site.html";
    fp = fopen(nome_arquivo_saida, "w");
    
    criarServidor(hints, pres, end);
    criarSocket(psock, res);    
    conectarServidor(sock_desc,res,end,fp);

    fclose(fp);
    close(sock_desc);

    ListaLinks *lista = buscarLinks(nome_arquivo_saida);
    lista = filtrar_lista(lista,end);

    print_lista(lista);

    return 0;
}