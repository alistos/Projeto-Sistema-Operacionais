#include <stdio.h>
#include <stdlib.h>
#include "analizador.h"

int main(int argc, char *argv[]) {
  ListaLinks *lista = buscarLinks(argv[1]);
  lista = filtrar_lista(lista,argv[2]);
  salvar_no_arquivo(lista);
  
  return 0;
}
