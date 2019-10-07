#include <stdio.h>
#include <stdlib.h>
#include "analizador.h"

int main(int argc, char const *argv[]) {
  ListaLinks *lista = buscarLinks("site.html");
  print_lista(lista);
  return 0;
}
