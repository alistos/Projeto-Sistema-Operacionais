#include "socketCrawler.h"

#define TRUE 1
#define FALSE 0
#define LENBUFFER 512
#define LEN_VETOR 1024

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

void conversarServidor(int sock_desc, struct addrinfo *res, char *endereco, char *subEndereco, char *caminho_arquivo){
    char msg[512]; //msg = mensagem que será enviada ao servidor
    char resp_servidor[LENBUFFER]; //variável que irá receber a resposta do servidor
    int bytes_read; //variável de suporte para capturar a resposta do servidor
    FILE *fp = fopen(caminho_arquivo, "w");
    if(fp == NULL){return;}
    //Enviar dados ao servidor
    //msg = "GET /index.html HTTP/1.1\r\nHost: www.site.com\r\n\r\n"; comando HTTP para pegar a pagina principal de um website.html
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
        bytes_read = recv(sock_desc, resp_servidor, LENBUFFER, 0);

        if(bytes_read == -1){
            perror("recv");
        }
        else{
            fprintf(fp, "%.*s", bytes_read, resp_servidor);
        }
    } while(bytes_read > 0);
    fclose(fp);

    //Checa se houve um erro de HTTPS, caso sim, tenta realizar uma conexão https agora
    FILE *arquivo = fopen(caminho_arquivo,"r");
    int c, i = 0;
    while(i<10){
	c = fgetc(arquivo);
	i++;
    }
    fclose(arquivo);
    if(c == '3' || c == '4'){
	int sockSSL_desc; //descritor do socket
        int *psock = &sockSSL_desc;
        criarServSockSSL(psock, endereco);
        conectarServidorSSL(psock, endereco, subEndereco, caminho_arquivo);
    }
}

//Conectar a um servidor remoto e conversar com ele
void conectarServidor(int sock_desc, struct addrinfo *res, char *endereco, char *subEndereco, char *caminho_arquivo){
    //connect(descritor do socket, struct do servidor, tamanho do servidor
    if(connect(sock_desc , res->ai_addr , res->ai_addrlen) < 0){
        printf("Nao foi possivel estabelecer a conexao!");
        perror("connect");
    }
    else{
        printf("Conexao estabelecida com sucesso!\n");
    }

    freeaddrinfo(res);
    conversarServidor(sock_desc, res, endereco, subEndereco, caminho_arquivo);
}

//definir a estrutura do socket servidor SSL
int criarServSockSSL(int *sock_desc, char *endereco){
    //É necessário concatenar o endereco recebido com https:// para que se possa capturar o endereco
    //https de forma correta
    char end[LENBUFFER] = "https://";
    strcat(end, endereco);
    char protocolo[10] = "";
    char nomeHost[LENBUFFER] = "";
    //O port padrão para conexões https é 443, ao contrário do port 80 para conexões http
    char numPort[10] = "443";
    int port;
    char *pointer = NULL;
    struct hostent *host;
    struct sockaddr_in dest_addr;

    //As instruções abaixo estão sendo realizadas para modificar a string do endereco de forma que ele 
    //seja usado de forma correta pelas funções que criarão o socket

    //remove o / no final do link caso exista
    if(end[LENBUFFER-1] == '/'){
        end[LENBUFFER-1] = '\0';
    }

    //O primeiro : termina a string do protocolo
    strncpy(protocolo, end, (strchr(end, ':')-end));

    //O hostname começa após o ://
    strncpy(nomeHost, strstr(end, "://")+3, sizeof(nomeHost));

    //Se o host possui um :, capturar o port
    if(strchr(nomeHost, ':')){
        pointer = strchr(nomeHost, ':');
        //O ultimo : começa o port
        strncpy(numPort, pointer+1, sizeof(numPort));
        *pointer = '\0';
    }

    port = atoi(numPort);

    if((host = gethostbyname(nomeHost)) == NULL){
       printf("Não foi possivel capturar o host %s.\n", nomeHost);
    }

    *sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(*sock_desc == -1){
        printf("Nao foi possivel criar o socket!");
        return *sock_desc;
    }

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = *(long*)(host->h_addr);

    memset(&(dest_addr.sin_zero), '\0', 8);
    pointer = inet_ntoa(dest_addr.sin_addr);

    if(connect(*sock_desc, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)) == -1){
        printf("Erro, nao foi possivel conectar-se ao host");
    }  
    return *sock_desc;
}

void conectarServidorSSL(int *sock_desc,char *endereco, char *subEndereco, char *caminho_arquivo){
    BIO *bioSaida = NULL;
    BIO *web;
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    SSL *ssl;
    int servidor = 0;
    
    //As funções abaixo inicializam as funcionalidades necessárias da biblioteca openssl
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
    SSL_load_error_strings();

    //Cria as BIOs de input e output
    bioSaida = BIO_new_fp(stdout, BIO_NOCLOSE);

    //Inicializa a biblioteca SSL
    if(SSL_library_init() < 0){
        BIO_printf(bioSaida, "Não foi possível iniciar a biblioteca OpenSSL!\n");
    }

    //Abaixo estão sendo implementados os passos para a criação objeto context, o SSL_CTX
    //Este objeto é usado para criar um novo objeto de conexão para cada conexão SSL, objetos
    //de conexão são usados para realizar handshakes, escritas e leituras

    //Faz com que o handshake tente utilizar o protocolo SSLv2, mas também coloca como opções 
    //o SSLv3 e TLSv1
    method = SSLv23_client_method();

    //Cria o objeto context
    if((ctx = SSL_CTX_new(method)) == NULL){
        BIO_printf(bioSaida, "Não foi possivel criar um objeto SSL!\n");
    }

    //Caso o protocolo SSLv2 não funcione, tenta negociar com SSLv3 e o TSLv1
    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);

    //Cria o objeto de estado SSL
    ssl = SSL_new(ctx);

    web = BIO_new_ssl_connect(ctx);
    BIO_get_ssl(web, &ssl);


    //Cria a conexão TCP
    servidor = criarServSockSSL(sock_desc, endereco);
    if(servidor == 0){
        BIO_printf(bioSaida, "Não foi possível criar a conexão TCP");
    }

    //Conecta a sessão SSL com o descritor do socket
    SSL_set_fd(ssl, servidor);

    //Realiza a conexão SSL
    if(SSL_connect(ssl)!=1){
        BIO_printf(bioSaida, "Não foi possivel reazilar a conexão SSL");
    }

    //Realizar a requisição HTTP para baixar as informações do site
    char msg[LENBUFFER]; //msg = mensagem que será enviada ao servidor
    char resp_servidor[LENBUFFER]; //variável que irá receber a resposta do servidor
    int bytes_read; //variável de suporte para capturar a resposta do servidor

    //Enviar dados ao servidor
    //msg = "GET /index.html HTTP/1.1\r\nHost: www.site.com\r\n\r\n"; comando HTTP para pegar a pagina 	    principal de um website
    //index.html fica subentendido quando não se coloca nada após o primeiro /
    //Host: precisa ser especificado pois vários endereços podem utilizar o mesmo servidor ip
    //Connection: close simplesmente fecha a conexão após a resposta do servidor ser enviada
    //char *subEndereco = NULL;
    if(subEndereco == NULL){
        strcpy(msg, "GET / HTTP/1.1\nHost: ");
    }else{
        strcpy(msg, "GET ");
        strcat(msg, subEndereco);
        strcat(msg, " HTTP/1.1\nHost: ");
    }
    
    strcat(msg, endereco);
    strcat(msg, "\r\nConnection: close\r\n\r\n");
    printf("%s\n",msg);
    BIO_puts(web, msg);
    BIO_puts(bioSaida, "\n");

    //a saida agora será direcionada para um arquivo chamado site.html
    bioSaida = BIO_new_file(caminho_arquivo, "w");

    //Receber resposta do servidor e escrever no arquivo usando as funções BIO_read que lê a resposta e 
    //a funcão BIO_write que escreve a resposta.
    int tam = 0;
    do{
        tam = BIO_read(web, resp_servidor, sizeof(resp_servidor));

        if(tam>0){
            BIO_write(bioSaida, resp_servidor, tam);
	}

    }while(tam>0 || BIO_should_retry(web));

    if(bioSaida!=NULL){
	BIO_free(bioSaida);
    }

    //Liberar as estruturas que não serão mais usadas
    SSL_free(ssl);
    close(servidor);
    SSL_CTX_free(ctx);
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
        printf("%sLINK: %s VISITADO!!!%s\n", BLUE, link, RESET);
    }
    printf("============================================\n");    
    return status;
}

ListaLinks* listar_links_visitados(char *dominio){
    ListaLinks *lista = startLista();
    char *buffer_link = malloc(LENBUFFER*sizeof(char));
    char *file_name = "linksVisitados.txt", *path = get_path(dominio, file_name);
    int fim;
    FILE *arquivo = fopen(path,"r");
    if(arquivo != NULL){
        printf("%s\n================ LINKS VISITADOS =================%s\n", YELLOW, GREEN);
        int i = 1;
        while(fgets(buffer_link, LENBUFFER, arquivo) != NULL){
            finalizar_string(buffer_link);
            printf("%d => %s \n", i, buffer_link);
            addLista(lista, buffer_link);
            buffer_link = malloc(LENBUFFER*sizeof(char));
            i++;
        }
    
        No *temp = lista->primeiro;
        while(temp!=NULL){
            printf("%s\n", temp->link);
            temp = temp->proximo;
        }
        printf("%s==================================================%s\n", YELLOW, RESET);

        fclose(arquivo);
    }

    return lista;
}

// Checa se um link já foi visitado.
int link_visitado(char *link, char *dominio){
    int boolean = FALSE;
    ListaLinks *lista = listar_links_visitados(dominio);
    char *link_lista = pop(lista);
    
    while (link_lista != NULL && boolean != TRUE){
        if(strcmp(link, link_lista) == 0){
            boolean = TRUE;
        }
        link_lista = pop(lista);
    }
    
    free_lista(lista);
    
    return boolean;
}

// Função para ser utilizado com threads. Faz download de uma pagina.
// O parametro passado é do tipo struct devido ser necessário mais de um
// argumento para a função.
// funções que são usadas com threads só tem um argumento do tipo void, que pode
// ser convertido internamento no tipo necessário a funcao, no caso uma struct.
void *baixar_pagina(void *args){

    Arg_download *arg = (Arg_download*)args;

    int sock_desc; //descritor do socket
    int *psock = &sock_desc;
    struct addrinfo hints, *res;
    struct addrinfo **pres = &res;

    char *caminho_arquivo = get_path(arg->endereco, arg->nome_arquivo_saida);
    
    criarServidor(hints, pres, arg->endereco);
    criarSocket(psock, res);    
    conectarServidor(sock_desc, res, arg->endereco, arg->subEndereco, caminho_arquivo);

    close(sock_desc);

    ListaLinks *lista = filtrar_lista(buscarLinks(caminho_arquivo), arg->endereco);
    ListaLinks *lista_link_arquivo = buscar_links_de_arquivo(lista, arg->endereco, arg->extensao_arquivo);

    printf("%s--------------------- LINKS COM EXTENSÃO %s ------------------------\n",BLUE, arg->extensao_arquivo );
    printf("----------------------- ENCOTRADOS %d ---------------------------\n",lista_link_arquivo->quantLinks);
    exibir_links_lista(lista_link_arquivo);
    printf("--------------------------------------------------------------------\n");
    
    printf("%s++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n", PINK);
    if(arg->subEndereco == NULL){
        printf("ARVORE DO LINKS (PAGINA PRINCIPAL) :%s %s %s\n", RED, arg->endereco, PINK);
    }
    else{
        printf("ARVORE DO LINKS :%s %s %s\n" , RED, arg->subEndereco, PINK);
    }
    print_lista(lista);
    exibir_links_lista(lista);
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n%s", RESET);

    salvar_links_econtrados(lista, arg->endereco);
}

// Percorre os links encontrados e salvos no arquivo.
void percorrer_links(char* dominio, char* tipo_arquivo){
    char buffer_link[LENBUFFER];
    char nome_arquivo_saida[LENBUFFER], temp[LENBUFFER];
    char *arq_links = "linksEncontrados.txt", *path = get_path(dominio, arq_links);
    int contador = 1;

    FILE *arquivoLinks = fopen(path,"r");
    if(arquivoLinks != NULL){
        while(fgets(buffer_link, LENBUFFER, arquivoLinks) != NULL){
            if(link_visitado(buffer_link, dominio) == FALSE){
                buffer_link[strlen(buffer_link)-1] = '\0';
                snprintf(temp, 10, "%d", contador);//converte int em string

                strcpy(nome_arquivo_saida, "link ");
                strcat(nome_arquivo_saida, temp);
                strcat(nome_arquivo_saida, ".html");

                Arg_download *args = start_arg(dominio, buffer_link, nome_arquivo_saida, tipo_arquivo);
                pthread_t thread;
                // Para cada link não visitado é criado um thred para baixa a respectiva página
                pthread_create(&thread,NULL,baixar_pagina,(void*)args);
                // Join cria sequencia e garante a executação de todos os threads.
                pthread_join(thread,NULL);
                contador++;
            
                salvar_link_visitado(buffer_link, dominio);
            }
        }
        fclose(arquivoLinks);
    }
}

// Cria a struct com parametros necessários para baixar uma página.
Arg_download* start_arg(char *endereco, char *subEndereco, char* nome_arquivo_saida, char *ext_arq){
    Arg_download *arg = malloc(sizeof(Arg_download));
    
    arg->endereco = endereco;
    arg->subEndereco = subEndereco;
    arg->nome_arquivo_saida = nome_arquivo_saida;
    arg->extensao_arquivo = ext_arq;

    return arg;
}

int criar_pasta_dominio(char *dominio){
    int result = mkdir(dominio, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    int boolean = FALSE;
    // Caso a pasta seja criada o retorno será 0(zero).
    if(!result){
        printf("%s============================================\n",GREEN);  
        printf("DIRETORIO %s CRIADO COM SUCESSO!!!\n",dominio);
        boolean = TRUE;
    }else{
        printf("%s============================================\n",RED);  
        printf("FALHA AO CRIAR DIRETORIO!!!\n");
    }
    
    printf("============================================%s\n", RESET);
    return boolean;
}

// Baixa a pagina inicial do dominio.
void *percorrer_dominio(void *args){
    Arg_percorrer_dominio *arg = (Arg_percorrer_dominio*)args;
    
    char *end = arg->dominio; //endereço do site a ser visitado
    char *nome_arquivo_saida = "site.html";
    char *tipo_arq = arg->tipo_arquivo;
    char *path = get_path(end, nome_arquivo_saida);

    criar_pasta_dominio(end);
    // Cria os argumentos necessário a função thread que baixa a página
    Arg_download *arg_site = start_arg(end, NULL, nome_arquivo_saida, arg->tipo_arquivo);

    pthread_t thread;
    // Criação do thread com a função requeira, e dando o cast para void pois só se pode
    // parra uma variavel como argumento. void* pode ser convetido internamente na funcao.
    pthread_create(&thread, NULL, baixar_pagina, (void*)arg_site);
    // Join garante que o thread será executado e cria uma sequencia de execução dos threads.
    pthread_join(thread, NULL);
            
    percorrer_links(end,arg->tipo_arquivo);
}

// cria a struct com paramentros necessários para percorrer um dominio.
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