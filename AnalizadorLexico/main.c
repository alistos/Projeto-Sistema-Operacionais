#include <stdio.h>
#include <stdlib.h>
#include "analizador.h"

int main(int argc, char *argv[]) {
  char *dominio = "www.youtube.com";
  char *link = "www.youtube.com/laksjklajs";
  printf("%d\n",contido_no_dominio(link,dominio));
  return 0;
}
