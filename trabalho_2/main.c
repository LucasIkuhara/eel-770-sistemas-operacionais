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

// Lock usada para restringir o acesso a variável contador
pthread_mutex_t contadorLock = PTHREAD_MUTEX_INITIALIZER;

// Locks que garantem que não existe ninguém do sexo oposto no banheiro

pthread_mutex_t semMulherLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t semHomemLock = PTHREAD_MUTEX_INITIALIZER;


// Função usada para representar o uso do banheiro
void usarBanheiro() {
    printf("Usando o banheiro.\n");
};

// Função que representa entrar no banheiro
// Sexo 0 = Feminino
// Sexo 1 = Masculino
void entrarNoBanheiro(int sexo) {

    // Caso o thread seja mulher
    if (sexo == 0) {
        while 
    }

    // Caso o thread seja homem
    else {

    }

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

        // Signal é chamado para acordar os threads esperando vaga no banheiro
        pthread_cond_signal(&cheioCond);

};

int main(void) {

    

    return 0;
}
