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

    criar_pasta_dominio(end);
    
    Arg_download *arg_site = start_arg(end, NULL,nome_arquivo_saida);

    pthread_t thread;
    pthread_create(&thread,NULL,baixar_pagina,(void*)arg_site);
    pthread_join(thread,NULL);
            
    //ListaLinks *lista = filtrar_lista(buscarLinks(nome_arquivo_saida),end);

    //print_lista(lista);
    //salvar_links_econtrados(lista);
    //percorrer_links(end);

    return 0;
}