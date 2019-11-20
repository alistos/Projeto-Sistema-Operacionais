#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strlen
#include <unistd.h> //close
#include <sys/socket.h>
#include <netdb.h> //struct addrinfo e função getaddrinfo
#include <pthread.h>
#include <sys/stat.h>
#include "socketCrawler.h"

#define TRUE 1
#define FALSE 0
#define LENBUFFER 1024

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

void conversarServidor(int sock_desc, struct addrinfo *res, char *endereco, char *subEndereco, FILE *fp){
    char msg[512]; //msg = mensagem que será enviada ao servidor
    char resp_servidor[1024]; //variável que irá receber a resposta do servidor
    int bytes_read; //variável de suporte para capturar a resposta do servidor

    //Enviar dados ao servidor
    //msg = "GET /index.html HTTP/1.1\r\nHost: www.site.com\r\n\r\n"; comando HTTP para pegar a pagina principal de um website
    //index.html fica subentendido quando não se coloca nada após o primeiro /
    //Host: precisa ser especificado pois vários endereços podem utilizar o mesmo servidor ip
    //Connection: close simplesmente fecha a conexão após a resposta do servidor ser enviada
    if(subEndereco == NULL){
        strcpy(msg, "GET / HTTP/1.1\nHost: ");
    }else{
        strcpy(msg, "GET ");
        strcat(msg, subEndereco);
        strcat(msg, " HTTP/1.1\nHost: ");
    }
    
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
void conectarServidor(int sock_desc, struct addrinfo *res, char *endereco, char *subEndereco, FILE *fp){
    //connect(descritor do socket, struct do servidor, tamanho do servidor
    if(connect(sock_desc , res->ai_addr , res->ai_addrlen) < 0){
        printf("Nao foi possivel estabelecer a conexao!");
        perror("connect");
    }
    else{
        printf("Conexao estabelecida com sucesso!\n");
    }

    freeaddrinfo(res);
    conversarServidor(sock_desc, res, endereco, subEndereco, fp);
}

int salvar_link_visitado(char *link, char *dominio){
    char *file_name = "linksVisitados.txt", *path = get_path(dominio, file_name);

    FILE *arquivo = fopen(path,"a");
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

ListaLinks* listar_links_visitados(char *dominio){
    ListaLinks *lista = startLista();
    char *buffer_link = malloc(LENBUFFER*sizeof(char));
    char *file_name = "linksVisitados.txt", *path = get_path(dominio, file_name);
    int fim;
    FILE *arquivo = fopen(path,"a");
    
    if(arquivo != NULL){
        while(fgets(buffer_link, LENBUFFER, arquivo) != NULL){
            fim = strlen(buffer_link)-1;
            buffer_link[fim] = '\0';
            addLista(lista, buffer_link);
            buffer_link = malloc(LENBUFFER*sizeof(char));
        }

        fclose(arquivo);
    }

    return lista;
}

int link_visitado(char *link, char *dominio){
    int boolean = FALSE;
    ListaLinks *lista = listar_links_visitados(dominio);
    char *link_lista = pop(lista);
    
    while (link_lista != NULL && boolean != TRUE){
        if(strcmp(link, link_lista) == 0){//funcao strcmp retorna 0(zero) quando as strings iguais
            boolean = TRUE;
        }
        link_lista = pop(lista);    
    }
    
    free_lista(lista);
    
    return boolean;
}

void *baixar_pagina(void *args){

    Arg_download *arg = (Arg_download*)args;

    int sock_desc; //descritor do socket
    int *psock = &sock_desc;
    struct addrinfo hints, *res;
    struct addrinfo **pres = &res;

    char *path = get_path(arg->endereco, arg->nome_arquivo_saida);
    
    FILE *fp; //arquivo onde será armazenado a resposta do servidor
    fp = fopen(path, "w");

    criarServidor(hints, pres, arg->endereco);
    criarSocket(psock, res);    
    conectarServidor(sock_desc, res, arg->endereco, arg->subEndereco, fp);

    fclose(fp);
    close(sock_desc);
}

void percorrer_links(char* dominio, char* tipo_arquivo){
    char *buffer_link = malloc(LENBUFFER*sizeof(char));
    char *nome_arquivo_saida = malloc(LENBUFFER*sizeof(char)), *temp = malloc(LENBUFFER*sizeof(char));
    char *arq_links = "linksEncontrados.txt", *path = get_path(dominio, arq_links);
    int contador = 1;

    FILE *arquivoLinks = fopen(path,"r");
    if(arquivoLinks != NULL){
        while(fgets(buffer_link, LENBUFFER, arquivoLinks) != NULL){
            if(!link_visitado(buffer_link, dominio)){
                buffer_link[strlen(buffer_link)-1] = '\0';
                snprintf(temp, 10, "%d", contador);//converte int em string

                strcpy(nome_arquivo_saida,"link ");
                strcat(nome_arquivo_saida,temp);
                strcat(nome_arquivo_saida,".html");

                Arg_download *args = start_arg(dominio, buffer_link, nome_arquivo_saida);
                pthread_t thread;
                pthread_create(&thread,NULL,baixar_pagina,(void*)args);
                pthread_join(thread,NULL);
                contador++;
            
                salvar_link_visitado(buffer_link, dominio);
                buscarLinks(get_path(dominio,nome_arquivo_saida));//percorre a pagina recem baixada para encontrar mais links
            }
        }
        fclose(arquivoLinks);
    }
}

Arg_download* start_arg(char *endereco, char *subEndereco, char* nome_arquivo_saida){
    Arg_download *arg = malloc(sizeof(Arg_download));
    
    arg->endereco = endereco;
    arg->subEndereco = subEndereco;
    arg->nome_arquivo_saida = nome_arquivo_saida;

    return arg;
}

void criar_pasta_dominio(char *dominio){
    int result = mkdir(dominio, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    // Caso a pasta seja criada o retorno será 0(zero).
    printf("============================================\n");  
    if(!result){
        printf("DIRETORIO %s CRIADO COM SUCESSO!!!\n",dominio);
    }else{
        printf("FALHA AO CRIAR DIRETORIO!!!\n");
    }
    printf("============================================\n");
}

void *percorrer_dominio(void *args){
    Arg_percorrer_dominio *arg = (Arg_percorrer_dominio*)args;
    char *end = arg->dominio; //endereço do site a ser visitado
    char* nome_arquivo_saida = "site.html";
    char *path = get_path(end, nome_arquivo_saida);

    criar_pasta_dominio(end);
        
    Arg_download *arg_site = start_arg(end, NULL,nome_arquivo_saida);

    pthread_t thread;
    pthread_create(&thread,NULL,baixar_pagina,(void*)arg_site);
    pthread_join(thread,NULL);
        
    ListaLinks *lista = filtrar_lista(buscarLinks(path),end);

    print_lista(lista);
    salvar_links_econtrados(lista,end);
    percorrer_links(end,arg->tipo_arquivo);
}

Arg_percorrer_dominio* start_arg_dominio(char *dominio, char *tipo_arquivo){
    Arg_percorrer_dominio *arg = malloc(sizeof(Arg_percorrer_dominio));
    arg->dominio = dominio;
    arg->tipo_arquivo = tipo_arquivo;
    return arg;
}

Arg_statistica* start_arg_statistica(char *dominio, char *tipo_arquivo){
    Arg_statistica* arg = malloc(sizeof(Arg_statistica));
    arg->dominio = dominio;
    arg->tipo_arquivo = tipo_arquivo;
    
    return arg;
}

void *exibir_statisticas(void *args){
    Arg_statistica* arg = (Arg_statistica*)args;
    char *str = buscar_links_de_arquivo(arg->dominio, arg->tipo_arquivo);
    printf("%s",str);
}