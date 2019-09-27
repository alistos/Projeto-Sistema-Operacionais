#include <stdio.h>
#include <stdlib.h>
#include "webcrawler.c"
#include <curl/curl.h>
//#include <tidy/tidy.h>
//#include <tidy/buffio.h>


int main(int argc, char **argv){

    printf("Essa é a URL fornecida: %s\n",argv[1]);
    //handle responsável por todas as operações do network
    CURL *handle;
    int result;
    FILE *fp;
    fp = fopen(argv[2], "wb");

    //inicia o handle
    handle = curl_easy_init();

    curl_easy_setopt(handle, CURLOPT_URL, argv[1]);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, fp);

    //inicia o download, retorna um int que diz se o download teve sucesso ou nao
    result = curl_easy_perform(handle);

    //Se o download foi efetuado com sucesso...
    if(CURLE_OK == result){
        curl_off_t value;

        //diz quantos bytes foram baixados
        result = curl_easy_getinfo(handle, CURLINFO_SIZE_DOWNLOAD_T, &value);
        if((CURLE_OK == result) && (value>0)){
            printf("Quantidade de dados baixada: %" CURL_FORMAT_CURL_OFF_T " bytes.\n", value);
        }

        result = curl_easy_getinfo(handle, CURLINFO_SPEED_DOWNLOAD_T, &value);
        if((CURLE_OK == result) && (value>0)){
            printf("AVelocidade média de download %" CURL_FORMAT_CURL_OFF_T
             " kbyte/sec.\n", value / 1024);
        }
    }


    fclose(fp);

    curl_easy_cleanup(handle);
}
