#ifndef SOCKETCRAWLER_H
#define SOCKETCRAWLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h> //struct addrinfo e função getaddrinfo
#include "AnalizadorLexico/analizador.h"

typedef struct arg_download_pagina{
    char *endereco;
    char *subEndereco;
    char *nome_arquivo_saida;
}Arg_download;

typedef struct arg_percorrer_dominio{
    char *dominio;
    char *tipo_arquivo;
}Arg_percorrer_dominio;

Arg_download* start_arg(char *endereco, char *subEndereco, char* nome_arquivo_saida);
Arg_percorrer_dominio* start_arg_dominio(char *dominio, char *tipo_arquivo);
struct addrinfo criarServidor(struct addrinfo hints, struct addrinfo **res, char *endereco);
int criarSocket(int *sock_desc, struct addrinfo *res);
void conversarServidor(int sock_desc, struct addrinfo *res, char *endereco, char *subEndereco, FILE *fp);
void conectarServidor(int sock_desc, struct addrinfo *res, char *endereco, char *subEndereco, FILE *fp);
int salvar_link_visitado(char *link, char *dominio);
ListaLinks* listar_links_visitados(char *dominio);
int link_visitado(char *link, char *dominio);
void *baixar_pagina(void *args);
void percorrer_links(char* dominio, char* tipo_arquivo);
void criar_pasta_dominio(char *dominio);
void *percorrer_dominio(void *dominio);

#endif