#include "socketCrawler.h"
#include "AnalizadorLexico/analizador.h"
#define LEN_BUFFER 312

int main(int argc, char *argv[]){
    char *tipo_arquivo = argv[1];

    if(argc < 3){
        printf("ARGUMENTO INSUFICIENTES!!!\n");
        printf("USO CORRETO => ./web.o tipo_de_arquivo dominio1 dominio2  dominio...\n");
        printf("EXEMPLO     => ./web.o gif www.exemplo.com www.exemplo2.com\n");
        return 1;
    }

    for(int i = 2 ; i<argc ; i++){
        char *dominio = argv[i];
        Arg_percorrer_dominio *args = start_arg_dominio(dominio,tipo_arquivo);
        Arg_statistica *arg_stat = start_arg_statistica(dominio,tipo_arquivo);
        
        pthread_t thread;
        pthread_create(&thread,NULL,percorrer_dominio,(void*)args);
        pthread_join(thread,NULL);
    }

    return 0;
}