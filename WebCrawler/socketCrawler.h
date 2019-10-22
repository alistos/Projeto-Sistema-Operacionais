#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr ou inet_aton


int criarSocket(int *sock_desc);
struct sockaddr_in criarServidor(struct sockaddr_in *servidor, char *endereco);
void conversarServidor(int sock_desc, struct sockaddr_in servidor, char *url, FILE *fp);
void conectarServidor(int sock_desc, struct sockaddr_in servidor, char *url, FILE *fp);

