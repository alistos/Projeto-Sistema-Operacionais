#include "analizador.h"

#define MAXBUFFER 1024
#define TRUE 1
#define FALSE 0

//Aloca espaco para um no de lista escadeada
No* startNo(char *link){
  No *novo = malloc(sizeof(No));
  novo->link = link;
  novo->proximo = NULL;

  return novo;
}

// Aloca espaco para uma lista encadeada
ListaLinks* startLista(){
  ListaLinks *lista = malloc(sizeof(ListaLinks));
  lista->primeiro = NULL;
  lista->ultimo = NULL;
  lista->quantLinks = 0;

  return lista;
}

// libera o espaco alocado para uma lista
void free_lista(ListaLinks *lista){
  char *link = pop(lista);
  while (link != NULL){
    free(link);
  }
  free(lista);
}

// Adiciona um link a uma lista encadeada
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

// Esta funcao retira um link salvo na lista e liberar a mémoria e retorna
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


// Exibir informações basicas sobre uma lista
void print_lista(ListaLinks *lista){
  printf("============================================\n");
  printf("LINKS ENCONTRADOS: %d\n", lista->quantLinks);
  printf("============================================\n");

}

// Função que recebe o nome de um arquivo, abre o mesmo e
// salva os links encontrados em uma lista encadeada
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

// Faz a coleta de um lista a partir de um arquivo
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

// Checa se um links pertence ao dominio requerido
int contido_no_dominio(char *link, char *dominio){
  int contido = FALSE;

  if(link[0] == '/' && link[1] != '/'){
    contido = TRUE;
  }
  else{
    if(strstr(link,dominio) != NULL){
      contido = TRUE;
    }
  }
  return contido;
}

// Filtra uma lista coletando apenas os links que pertencem ao dominio especificado
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

// Coloca o simbolo de fim de string para evitar erros.
void finalizar_string(char *string){
  int i = 0;
  while (string[i] != '\n'){
          i++;
  }
  string[i] = '\0';
}

int salvar_links_econtrados(ListaLinks *lista, char *dominio){
  char *file_name = "linksEncontrados.txt", *path = get_path(dominio, file_name);
  char *buffer_link = malloc(MAXBUFFER*sizeof(char));
  FILE *arquivo;
  ListaLinks *links_salvos = startLista();
  
  arquivo = fopen(path,"r");
  if (arquivo != NULL){
      while (fgets(buffer_link, MAXBUFFER,arquivo) != NULL){
        finalizar_string(buffer_link);
        addLista(links_salvos, buffer_link);
        buffer_link = malloc(MAXBUFFER*sizeof(char));
      }

      fclose(arquivo);
  }
  
  arquivo = fopen(path,"a");
  
  int status = TRUE, quantLinks = lista->quantLinks;
  
  if(arquivo == NULL){
    status = FALSE;
  }
  else{
    char* link = pop(lista);
    No *temp;
    int foi_salvo = FALSE;
    while(link!=NULL){
      temp = links_salvos->primeiro;
      while(temp != NULL && foi_salvo != TRUE){
        if(strcmp(temp->link, link) == 0){
          foi_salvo = TRUE;
        }
        temp = temp->proximo;
      }

      if(foi_salvo == FALSE){
        fprintf(arquivo,"%s\n", link);
      }
      link = pop(lista);
    }

    free(lista);
    fclose(arquivo);
  }

  printf("============================================\n");
  if(status){
    printf("%d LINKS ADICIONADOS NO ARQUIVO!!!\n", quantLinks);
  }else{
    printf("ERRO AO ADICIONAR LINKS NO ARQUIVO!!!\n");
  }
  printf("============================================\n");

  return status;
}

//esta fucao retorna o caminho para abertura de um arquivo na pasta do dominio.
char* get_path(char *dominio, char *file_name){
    char *path = malloc(MAXBUFFER*sizeof(char));

    strcpy(path, dominio);
    strcat(path, "/");
    strcat(path, file_name);

    return path;
}
// procura links que possuem a extenção de arquivo passada como argumento
ListaLinks* buscar_links_de_arquivo(ListaLinks *lista, char* dominio ,char *extensao_arquivo){
  ListaLinks *lista_links_arquivo = startLista();
  No *no = lista->primeiro;

  char *arq = malloc(MAXBUFFER*sizeof(char));
  strcpy(arq, extensao_arquivo);
  strcat(arq,".txt");

  FILE *arquivo = fopen(get_path(dominio, arq), "a");

  while(no != NULL){
    if(strstr(no->link, extensao_arquivo)){
      char *buffer = malloc(strlen(no->link)+1);
      strcpy(buffer, no->link);
      addLista(lista_links_arquivo, buffer);
      fprintf(arquivo,"%s\n", no->link);
    }
    no = no->proximo;
  }
  return lista_links_arquivo;
}

// Exibi em console todos os links de uma lista
void exibir_links_lista(ListaLinks *lista){
  No *no = lista->primeiro;
  while(no != NULL){
    printf("%s\n", no->link);
    printf("---------------------------------------------------------------------\n");
    no = no->proximo;
  }
}