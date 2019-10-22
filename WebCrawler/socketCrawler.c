#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr ou inet_aton
#include "socketCrawler.h"
#include <unistd.h> //close

//Criar o socket
//socket(dominio, tipo de socket, protocolo)
//AF_INET = ipv4, SOCK_STREAM = TCP, protocolo 0 é o padrão
int criarSocket(int *sock_desc){
    *sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(*sock_desc == -1){
        printf("Nao foi possivel criar o socket!");
        return *sock_desc;
    }
    return *sock_desc;
}

//definir a estrutura do socket servidor
struct sockaddr_in criarServidor(struct sockaddr_in *servidor, char *endereco){
    servidor->sin_family = AF_INET;
    servidor->sin_port = htons(80); //port 80 é onde a maioria dos webservers rodam
    //a função inet_aton envia o IP convertido para seu equivalente hexadecimal para o servidor
    inet_aton(endereco, &servidor->sin_addr);

    return *servidor;
}

void conversarServidor(int sock_desc, struct sockaddr_in servidor, char *url, FILE *fp){
    char msg[100]; //msg = mensagem que será enviada ao servidor
    char resp_servidor[1024]; //variável que irá receber a resposta do servidor
    int bytes_read; //variável de suporte para capturar a resposta do servidor

    //Enviar dados ao servidor
    //msg = "GET /index.html HTTP/1.1\r\nHost: www.site.com\r\n\r\n"; comando HTTP para pegar a pagina principal de um website
    //index.html fica subentendido quando não se coloca nada após o primeiro /
    //Host: precisa ser especificado pois vários endereços podem utilizar o mesmo servidor ip
    //Connection: close simplesmente fecha a conexão após a resposta do servidor ser enviada
    strcpy(msg, "GET / HTTP/1.1\r\nHost: ");
    strcat(msg, url);
    strcat(msg, "\r\nConnection: close\r\n\r\n");
    printf("%s\n",msg);
    if(send(sock_desc, msg, strlen(msg), 0) < 0){
        printf("Nao foi possivel enviar os dados!");
    }
    else{
        printf("Os dados foram enviados com sucesso!\n");
    }

    //Receber resposta do servidor
    //recv(descritor do socket, variável onde será armazenada a resposta do servidor, tamanho da variável, falgs, padrão 0)
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
void conectarServidor(int sock_desc, struct sockaddr_in servidor, char *url, FILE *fp){
    //connect(descritor do socket, struct do servidor com cast sockaddr, tamanho do servidor
    if(connect(sock_desc , (struct sockaddr *)&servidor , sizeof(servidor)) < 0){
        printf("Nao foi possivel estabelecer a conexao!");
        perror("connect");
    }
    else{
        printf("Conexao estabelecida com sucesso!\n");
    }

    conversarServidor(sock_desc, servidor, url, fp);

}
