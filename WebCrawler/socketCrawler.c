#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strlen
#include <unistd.h> //close
#include <sys/socket.h>
#include <netdb.h> //struct addrinfo e função getaddrinfo
#include "socketCrawler.h"




//definir a estrutura do socket servidor
struct addrinfo criarServidor(struct addrinfo hints, struct addrinfo **res, char *endereco){
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if(getaddrinfo(endereco, "80", &hints, res)!=0){
        perror("getaddrinfo");
    }


    return **res;
}

//Criar o socket
//socket(dominio, tipo de socket, protocolo)
//AF_INET = ipv4, SOCK_STREAM = TCP, protocolo 0 é o padrão
int criarSocket(int *sock_desc, struct addrinfo *res){

    *sock_desc = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(*sock_desc == -1){
        printf("Nao foi possivel criar o socket!");
        return *sock_desc;
    }
    return *sock_desc;
}

void conversarServidor(int sock_desc, struct addrinfo *res, char *endereco, FILE *fp){
    char msg[100]; //msg = mensagem que será enviada ao servidor
    char resp_servidor[1024]; //variável que irá receber a resposta do servidor
    int bytes_read; //variável de suporte para capturar a resposta do servidor

    //Enviar dados ao servidor
    //msg = "GET /index.html HTTP/1.1\r\nHost: www.site.com\r\n\r\n"; comando HTTP para pegar a pagina principal de um website
    //index.html fica subentendido quando não se coloca nada após o primeiro /
    //Host: precisa ser especificado pois vários endereços podem utilizar o mesmo servidor ip
    //Connection: close simplesmente fecha a conexão após a resposta do servidor ser enviada
    strcpy(msg, "GET / HTTP/1.1\nHost: ");
    strcat(msg, endereco);
    strcat(msg, "\r\nConnection: close\n\n");
    printf("%s\n",msg);
    if(send(sock_desc, msg, strlen(msg), 0) < 0){
        printf("Nao foi possivel enviar os dados!");
    }
    else{
        printf("Os dados foram enviados com sucesso!\n");
    }

    //Receber resposta do servidor
    //recv(descritor do socket, variável onde será armazenada a resposta do servidor, tamanho da variável, flags, padrão 0)
    //do while usado para que enquanto a resposta do servidor não for completamente capturada, continuar pegando o que falta
    do{
        bytes_read = recv(sock_desc, resp_servidor, 1024, 0);

        if(bytes_read == -1){
            perror("recv");
        }
        else{
            fprintf(fp, "%.*s", bytes_read, resp_servidor);
        }
    } while(bytes_read > 0);
    printf("%s\n",resp_servidor);
}

//Conectar a um servidor remoto e conversar com ele
void conectarServidor(int sock_desc, struct addrinfo *res, char *endereco, FILE *fp){
    //connect(descritor do socket, struct do servidor, tamanho do servidor
    if(connect(sock_desc , res->ai_addr , res->ai_addrlen) < 0){
        printf("Nao foi possivel estabelecer a conexao!");
        perror("connect");
    }
    else{
        printf("Conexao estabelecida com sucesso!\n");
    }

    freeaddrinfo(res);
    conversarServidor(sock_desc, res, endereco, fp);

}

int salvar_link_visitado(char *link){
    FILE *arquivo = fopen("linksVisitados.txt","a");
    int status = TRUE;

    printf("============================================\n");  
    if(arquivo == NULL){
        printf("ERRO AO ADICIONAR LINK NO ARQUIVO!!!\n");
        status = FALSE;
    }else{
        fprintf(arquivo,"%s\n", link);  
        fclose(arquivo);      
        printf("LINK: %s VISITADO!!!\n", link);
    }
    printf("============================================\n");    
    return status;
}