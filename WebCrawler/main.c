#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include "socketCrawler.h"
#include "analizador.h"
#include <unistd.h> //close
#include <netdb.h> //struct addrinfo e função getaddrinfo

int main(int argc, char *argv[]){
     		
    char *end = argv[1]; //endereço do site a ser visitado
    char* nome_arquivo_saida = "site.html";
    
    baixar_pagina(end, nome_arquivo_saida);

    ListaLinks *lista = filtrar_lista(buscarLinks(nome_arquivo_saida),end);

    print_lista(lista);
    salvar_links_econtrados(lista);

    return 0;
}