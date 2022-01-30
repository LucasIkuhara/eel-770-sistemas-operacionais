#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


// Variáveis de condição para o sexo oposto entrar no banheiro
pthread_cond_t  semMulherCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t  semHomemCond = PTHREAD_COND_INITIALIZER;

// Variável de condição para banheiro cheio
pthread_cond_t  cheioCond = PTHREAD_COND_INITIALIZER;

// Armazena a quantidade de pessoas no banheiro em um dado momento
int qtdNoBanheiro = 0;

// Armazena sexo usando o banheiro (0 indica mulher, 1 indica homem, 2 indica que está vazio)
int sexoDoUsuario = 2;

// Lock usada para restringir o acesso a variável contador
pthread_mutex_t contadorLock = PTHREAD_MUTEX_INITIALIZER;

// Locks que garantem que não existe ninguém do sexo oposto no banheiro
pthread_mutex_t semMulherLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t semHomemLock = PTHREAD_MUTEX_INITIALIZER;


// Função usada para representar o uso do banheiro
void usarBanheiro() {
    printf("Usando o banheiro.\n");
};

// Função que representa entrar no banheiro para threads mulher
void entrarNoBanheiroMulher() {

    while (sexoDoUsuario != 0 || sexoDoUsuario != 2) {
    
        // Espera até que nenhum homem esteja no banheiro
        pthread_cond_wait(&semHomemCond, &semHomemLock);
    }
    
    // Ativa o lock semMulher, para que nenhum homem possa entrar e garante que a variável
    // reflete a presença de uma mulher no banheiro
    pthread_mutex_lock(&semMulherLock);
    sexoDoUsuario = 0;

    // Threads esperam enquanto o banheiro está cheio (3 ou mais pessoas)
    while(qtdNoBanheiro >= 3) {
        pthread_cond_wait(&cheioCond, &contadorLock);
    }
        // Incrementa o contador, tendo acesso exclusivo a variável contador
        pthread_mutex_lock(&contadorLock);
        qtdNoBanheiro++;

        // O mutex do contador é desbloqueado para que mais de uma pessoa consiga de fato
        // usar o banheiro por vez (Ou seja a função usarBanheiro pode ser chamada por mais de
        // um thread que já tenha acesso a zona crítica)
        pthread_mutex_unlock(&contadorLock);

        usarBanheiro();
        
        // Decrementa o contador de pessoas no banheiro
        pthread_mutex_lock(&contadorLock);
        qtdNoBanheiro--;
        pthread_mutex_unlock(&contadorLock);

        // Se ninguém estiver no banheiro, desabilitar travas de sexo, e muda o indicador de flag para 2 (vazio)
    	if (qtdNoBanheiro == 0) {
            pthread_mutex_unlock(&semMulherLock);
            sexoDoUsuario = 2;
        }

        // Signal é chamado para acordar um thread esperando que o último membro do sexo oposto deixe o banheiro
        pthread_cond_signal(&semMulherCond);

        // Signal é chamado para acordar um thread esperando vaga no banheiro
        pthread_cond_signal(&cheioCond);

        // Ao chamarmos o signal do lock de sexo antes do de quantidade, criamos uma ideia de revezamento entre
        // os sexos, visando evitar starvation

        // Thread finaliza
        pthread_exit(NULL);
};

int main(void) {


    return 0;
}
