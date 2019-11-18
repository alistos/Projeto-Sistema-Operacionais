# Projeto-Sistema-Operacionais
- Como compilar:
gcc AnalizadorLexico/*.c *.c -o web.o -pthread -lssl -lcrypto
- Como usar:  
./web.o extessão_arquivo dominio1 dominio2 dominio...  
Exemplo:  
./web.o gif www.exemplo.com www.exemplo2.com  
 
Mais de um dominio pode ser passado como argumento
Detalhe: Alguns sites precisam ser chamados no terminal sem o www para funcionar corretamente, exemplo, kotaku.com precisa ser chamado dessa forma, e não www.kotaku.com


