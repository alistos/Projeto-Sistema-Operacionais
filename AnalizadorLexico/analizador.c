#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analizador.h"
#define MAXBUFFER 512
#define TRUE 1
#define FALSE 0

No* startNo(char *link){
  No *novo = malloc(sizeof(No));
  novo->link = link;
  novo->proximo = NULL;

  return novo;
}

ListaLinks* startLista(){
  ListaLinks *lista = malloc(sizeof(ListaLinks));
  lista->primeiro = NULL;
  lista->ultimo = NULL;
  lista->quantLinks = 0;

  return lista;
}

void addLista(ListaLinks* lista, char* link){
  No *novo = startNo(link);
  if(lista->primeiro == NULL){
    lista->primeiro = novo;
    lista->ultimo = novo;
  }
  else{
    lista->ultimo->proximo = novo;
    lista->ultimo = novo;
  }
  lista->quantLinks += 1;
}

char* pop(ListaLinks *lista){
  char* link = NULL;
  if(lista->primeiro != NULL){
    link = lista->primeiro->link;

    No *temp = lista->primeiro;

    lista->primeiro = temp->proximo;
    lista->quantLinks -= 1;
    free(temp);
    if(lista->primeiro == NULL){
      lista->ultimo = NULL;
    }
  }
  return link;
}

void print_lista(ListaLinks *lista){
  No *no = lista->primeiro;
  printf("LINKS ENCONTRADOS: %d\n", lista->quantLinks);
  while(no!=NULL){
    printf("%s\n",no->link);
    no = no->proximo;
  }
}

ListaLinks* buscarLinks(char* nome_arquivo){
  FILE *arquivo = fopen(nome_arquivo,"r");
  ListaLinks *lista = startLista();
  char c = getc(arquivo), *href = "href";
  int len_href = strlen(href), i;
  char *string = malloc(len_href*sizeof(char)+1);
  
  for(i = 0;i<len_href;i++){
    string[i] = c;
    c = getc(arquivo);
  }
  string[i++] = '\0';

  while (c!=EOF){
    if(strcmp(string,href)==0){
      addLista(lista,pegar_link(arquivo));
    }
    for(i = 0;i<(len_href-1);i++){
      string[i] = string[i+1];
    }
    c = getc(arquivo);
    string[i] = c;
  }
  
  fclose(arquivo);
  return lista;
}

char* pegar_link(FILE *arquivo){
  char *link = malloc(MAXBUFFER*sizeof(char)), c = getc(arquivo);

  while(c!='"'){c = getc(arquivo);}
  for(int i = 0;i<MAXBUFFER;i++){
    link[i] = getc(arquivo);
    if(link[i]=='"'){
      link[i] = '\0';
      return link;
    }
  }
}

int contido_no_dominio(char *link, char *dominio){
  int contido = FALSE;

  if(link[0] == '/' && link[1] != '/'){
    contido = TRUE;
  }
  else{
    int len_dominio = strlen(dominio);
    char *dominio_link = malloc(len_dominio*sizeof(char));
    strncpy(dominio_link,link,len_dominio);

    if(strstr(link,dominio) != NULL){
      contido = TRUE;
    }
  }
  return contido;
}

ListaLinks* filtrar_lista(ListaLinks *lista, char *dominio){
  ListaLinks *lista_filtrada = startLista();
  No *no = lista->primeiro, *temp;

  while(no!=NULL){
    if(contido_no_dominio(no->link,dominio)){
      addLista(lista_filtrada,no->link);
    }
    temp = no;
    no = no->proximo;
    free(temp);
  }
  free(lista);
  return lista_filtrada;
}

int salvar_no_arquivo(ListaLinks *lista){
  FILE *arquivo = fopen("linksEncontrados.txt","a");
  int status = TRUE;

  if(arquivo == NULL){
    status = FALSE;
  }

  char* link = pop(lista);
  while(link!=NULL){
    fprintf(arquivo,"%s\n",link);
    link = pop(lista);
  }

  free(lista);
  fclose(arquivo);
  return status;