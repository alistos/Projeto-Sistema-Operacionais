#ifndef SOCKETCRAWLER_H
#define SOCKETCRAWLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include <netdb.h> //struct addrinfo e função getaddrinfo

struct addrinfo criarServidor(struct addrinfo hints, struct addrinfo **res, char *endereco);
int criarSocket(int *sock_desc, struct addrinfo *res);
void conversarServidor(int sock_desc, struct addrinfo *res, char *endereco, FILE *fp);
void conectarServidor(int sock_desc, struct addrinfo *res, char *endereco, FILE *fp);

#endif