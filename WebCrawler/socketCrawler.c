#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> //inet_aton
#include <netdb.h>

#include <unistd.h>
#include <pthread.h>

struct arg_struct{
    void *arg1;
    struct sockaddr_in arg2;
}args;

void *thread_con(void *);

int main(int argc, char *argv[]){

    int sock_desc, con_sock, *thr_sock;
    struct sockaddr_in servidor, cliente;
    char *msg;
    //char *end = argv[1];
    int tamstr = sizeof(struct sockaddr_in);

    //Criar o socket
    //socket(dominio, tipo de socket, protocolo)
    //AF_INET = ipv4, SOCK_STREAM = TCP
    sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_desc == -1){
        printf("Nao foi possivel criar o socket!");
    }

    //definir a estrutura do servidor
    //servidor.sin_addr.s_addr = inet_addr(end);
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(8888); //port 80 é onde a maioria dos webservers rodam
    //inet_aton(end, &servidor.sin_addr); //a função inet_aton envia o IP para o servidor

    //Realiza o Bind do servidor
    if(bind(sock_desc,(struct sockaddr *)&servidor, sizeof(servidor))==-1){
        printf("Nao foi possivel realizar o bind do servidor\n");
    }
    else{
        printf("Bind realizado!\n");
    }

    //escuta
    listen(sock_desc, 3);

    //Aceitar novas conexões
    printf("Esperando por novas conexões: \n");
    while((con_sock = accept(sock_desc, (struct sockaddr *)&cliente, (socklen_t *)&tamstr))){
        printf("Conexao realizada com sucesso!\n");

        //Responder ao socket cliente
        msg = "Olá, sua conexao foi aceita!\n";
        write(con_sock, msg, strlen(msg));

        pthread_t thr;
        thr_sock= malloc(1);
        *thr_sock= con_sock;


        struct arg_struct *args = malloc(sizeof(struct arg_struct));
        args->arg1 = thr_sock;
        args->arg2 = servidor;

        if(pthread_create(&thr, NULL, thread_con, (void*)&args)!=0){
            perror("Nao foi possivel criar a thread");
            return 0;
        }

    }

    return 0;
}

void *thread_con(void *arguments){
    struct arg_struct *args = arguments;
    int sock = *(int*)args->arg1;
    struct sockaddr_in servidor = args->arg2;
    char *msg, cli_msg[10000], resp_servidor[10000];

    //mensagens para o cliente
    msg = "Olá, por favor, indique qual o site que voce quer avaliar";
    write(sock, msg, strlen(msg));
    char *end;
    scanf("%s", end);

    servidor.sin_addr.s_addr = inet_addr(end);
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(80);


        //Conectar a um servidor remoto
    if(connect(sock, (struct sockaddr *)&servidor , sizeof(servidor)) < 0){
        printf("Nao foi possivel estabelecer a conexao!");
        return 0;
    }
    else{
        printf("Conexao estabelecida com sucesso!\n");
    }

    //Enviar dados ao servidor
    msg = "GET / HTTP/1.1\r\n\r\n"; //comando HTTP para pegar a pagina principal de um website
    if(send(sock, msg, strlen(msg), 0) < 0){
        printf("Nao foi possivel enviar os dados!");
        return 0;
    }
    else{
        printf("Os dados foram enviados com sucesso!\n");
    }

    //Receber resposta do servidor
    if(recv(sock, resp_servidor, 10000, 0) < 0){
        printf("Nao foi possivel recuperar a resposta do servidor!");
        return 0;
    }
    else{
        printf("Resposta recuperada com sucesso!\n");
    }
    puts(resp_servidor);
}
