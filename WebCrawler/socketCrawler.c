#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <arpa/inet.h> //inet_aton
#include "socketCrawler.h"
#include <unistd.h>
#include <pthread.h>

int main(int argc, char *argv[]){
    int sock_desc;
    struct sockaddr_in servidor;
    char msg[100], resp_servidor[10000];
    char *end = argv[1];
    char *url = argv[2];
    int bytes_read;
    FILE *fp;
    fp = fopen("site.html", "w");

    //Criar o socket
    //socket(dominio, tipo de socket, protocolo)
    //AF_INET = ipv4, SOCK_STREAM = TCP
    sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_desc == -1){
        printf("Nao foi possivel criar o socket!");
    }

    //definir a estrutura do socket servidor
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(80); //port 80 é onde a maioria dos webservers rodam
    inet_aton(end, &servidor.sin_addr); //a função inet_aton envia o IP para o servidor

    //Conectar a um servidor remoto
    if(connect(sock_desc , (struct sockaddr *)&servidor , sizeof(servidor)) < 0){
        printf("Nao foi possivel estabelecer a conexao!");
        return 0;
    }
    else{

        printf("Conexao estabelecida com sucesso!\n");
    }


    //Enviar dados ao servidor
    //msg = "GET /index.html HTTP/1.1\r\nHost: www.google.com\r\n\r\n"; //comando HTTP para pegar a pagina principal de um website
    strcpy(msg, "GET / HTTP/1.1\r\nHost: ");
    strcat(msg, url);
    strcat(msg, "\r\nConnection: close\r\n\r\n");
    printf("%s\n",msg);
    if(send(sock_desc, msg, strlen(msg), 0) < 0){
        printf("Nao foi possivel enviar os dados!");
        return 0;
    }
    else{
        printf("Os dados foram enviados com sucesso!\n");
    }
    //Receber resposta do servidor
    do{
        bytes_read = recv(sock_desc, resp_servidor, 10000, 0);
        if(bytes_read == -1){
            perror("recv");
        }
        else{
            fprintf(fp, "%.*s", bytes_read, resp_servidor);
        }
    } while(bytes_read > 0);
    printf("%s\n",resp_servidor);
    fclose(fp);
    close(sock_desc);

    return 0;
}
