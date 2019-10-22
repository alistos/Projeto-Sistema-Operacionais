#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define MAXTHREADS 10
#define MAX_VETOR 100

typedef struct argumentos{
   int inicio;
   int fim;
   int* vetor;
}Args;

void *exibir_vetor(void *args){
  Args* argumentos = (Args*)args;

  for(int k = argumentos->inicio;k<argumentos->fim;k++){
    if(k%10 == 0){printf("\n");}
    if(argumentos->vetor[k]<10){printf("0%d ",argumentos->vetor[k]);}
    else{printf("%d ",argumentos->vetor[k]);}
  }
  printf("\n");
}

void *preenche_vetor(void *args){
  Args* argumentos = (Args*)(args);
  int cont = argumentos->inicio;
  for(int i = argumentos->inicio;i<=argumentos->fim;i++){
    argumentos->vetor[i] = (cont++);
  }
}

int main(int argc, char const *argv[]){
  int vetor[MAX_VETOR];
  Args* vetor_args[10];
  pthread_t threads[MAXTHREADS];
  int fim_ant = -1;

  for(int i=0;i<MAXTHREADS;i++){
    Args *args = malloc(sizeof(Args));
    args->inicio = fim_ant+1;
    args->fim = fim_ant+10;
    args->vetor = vetor;
    fim_ant = args->fim;
    vetor_args[i] = args;
  }

  for(int j=0;j<MAXTHREADS;j++){
    pthread_t thread;
    pthread_create(&thread,NULL,preenche_vetor,(void*)vetor_args[j]);
    threads[j] = thread;
  }

  for(int k=0;k<MAXTHREADS;k++){pthread_join(threads[k],NULL);}

  pthread_t thread_exibir;
  Args arguments = {0,MAX_VETOR,vetor};

  pthread_create(&thread_exibir,NULL,exibir_vetor,(void*)(&arguments));
  pthread_join(thread_exibir,NULL);

  pthread_exit(NULL);
}
