#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>



/*
    O problema escolhido for: The unisex bathroom problem (Seção 6.2).
    Os threads são criados de maneira que seus sexos são alternados. (Um mulher, um homem)
    O programa foi desenvolvido e testado no sistema operacional Ubuntu 20.04 rodando no WSL2
    A função de usar o banheiro imprime uma frase e espera um tempo entre 0.5 e 2 segundos.
    A quantidade de threads utilizada pode ser definida pela macro abaixo:
*/
#define THREADS 10

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


// Funções usadas para representar o uso do banheiro
void usarBanheiroMulher() {   
    printf("[Mulher]: Usando o banheiro.\n");
    
    // Espera entre 0.5s e 2s 
    sleep(0.5 + ((float)rand()/(float)(RAND_MAX)) * 1.5);
};

void usarBanheiroHomem() {   
    printf("[Homem]: Usando o banheiro.\n");
    
    // Espera entre 0.5s e 2s 
    sleep(0.5 + ((float)rand()/(float)(RAND_MAX)) * 1.5);
};


// Função que representa entrar no banheiro para threads mulher
void entrarNoBanheiroMulher() {
        printf("[Mulher]: Iniciando Thread Mulher.\n");

    // while (sexoDoUsuario != 1 && sexoDoUsuario != 2) {

    //     printf("[Mulher]: Esperando não terem homemes no banheiro.\n");
    //     // Espera até que nenhuma homem esteja no banheiro
    //     pthread_cond_wait(&semhomemCond, &semhomemLock);
    // }
    
    // Ativa o lock semMulher, para que nenhuma homem possa entrar e garante que a variável
    // reflete a presença de um Mulher no banheiro
    //pthread_mutex_lock(&semMulherLock);
    sexoDoUsuario = 1;

    // Travar o lock para proteger a variável de contador checada no while
    pthread_mutex_lock(&contadorLock);

    // Threads esperam enquanto o banheiro está cheio (3 ou mais pessoas)
    while(qtdNoBanheiro >= 3) {
        printf("[Mulher]: Esperando o banheiro não estar cheio.\n");
        pthread_cond_wait(&cheioCond, &contadorLock);
    }
        
        // O lock não é liberado aqui, pois esperamos o contador ser aumentado, evitando desbloquear e
        // bloquear o lock atoa.

        // Incrementa o contador, tendo acesso exclusivo a variável contador      
        qtdNoBanheiro++;
        printf("[Mulher]: Entrando no banheiro. Agora tem %d pessoas no banheiro.\n", qtdNoBanheiro);

        // O mutex do contador é desbloqueado para que mais de uma pessoa consiga de fato
        // usar o banheiro por vez (Ou seja a função usarBanheiro pode ser chamada por mais de
        // um thread que já tenha acesso a zona crítica)
        pthread_mutex_unlock(&contadorLock);

        usarBanheiroMulher();
        
        // Decrementa o contador de pessoas no banheiro
        pthread_mutex_lock(&contadorLock);
       
        qtdNoBanheiro--;
        printf("[Mulher]: Saindo do banheiro. Agora tem %d pessoas no banheiro.\n", qtdNoBanheiro);
       
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

// Função que representa entrar no banheiro para threads homem
void entrarNoBanheiroHomem() {
    printf("[Homem]: Iniciando Thread homem.\n");

    // while (sexoDoUsuario != 1 && sexoDoUsuario != 2) {

    //     printf("[Homem]: Esperando não terem mulheres no banheiro.\n");
    //     // Espera até que nenhuma mulher esteja no banheiro
    //     pthread_cond_wait(&semMulherCond, &semMulherLock);
    // }
    
    // Ativa o lock semHomem, para que nenhuma mulher possa entrar e garante que a variável
    // reflete a presença de um homem no banheiro
    //pthread_mutex_lock(&semHomemLock);
    sexoDoUsuario = 1;

    // Travar o lock para proteger a variável de contador checada no while
    pthread_mutex_lock(&contadorLock);

    // Threads esperam enquanto o banheiro está cheio (3 ou mais pessoas)
    while(qtdNoBanheiro >= 3) {
        printf("[Homem]: Esperando o banheiro não estar cheio.\n");
        pthread_cond_wait(&cheioCond, &contadorLock);
    }
        
        // O lock não é liberado aqui, pois esperamos o contador ser aumentado, evitando desbloquear e
        // bloquear o lock atoa.

        // Incrementa o contador, tendo acesso exclusivo a variável contador      
        qtdNoBanheiro++;
        printf("[Homem]: Entrando no banheiro. Agora tem %d pessoas no banheiro.\n", qtdNoBanheiro);

        // O mutex do contador é desbloqueado para que mais de uma pessoa consiga de fato
        // usar o banheiro por vez (Ou seja a função usarBanheiro pode ser chamada por mais de
        // um thread que já tenha acesso a zona crítica)
        pthread_mutex_unlock(&contadorLock);

        usarBanheiroHomem();
        
        // Decrementa o contador de pessoas no banheiro
        pthread_mutex_lock(&contadorLock);
       
        qtdNoBanheiro--;
        printf("[Homem]: Saindo do banheiro. Agora tem %d pessoas no banheiro.\n", qtdNoBanheiro);
       
        pthread_mutex_unlock(&contadorLock);

        // Se ninguém estiver no banheiro, desabilitar travas de sexo, e muda o indicador de flag para 2 (vazio)
    	if (qtdNoBanheiro == 0) {
            pthread_mutex_unlock(&semHomemLock);
            sexoDoUsuario = 2;
        }

        // Signal é chamado para acordar um thread esperando que o último membro do sexo oposto deixe o banheiro
        pthread_cond_signal(&semHomemCond);

        // Signal é chamado para acordar um thread esperando vaga no banheiro
        pthread_cond_signal(&cheioCond);

        // Ao chamarmos o signal do lock de sexo antes do de quantidade, criamos uma ideia de revezamento entre
        // os sexos, visando evitar starvation

        // Thread finaliza
        pthread_exit(NULL);
};


int main(void) {
    
    // Referências:
    // https://stackoverflow.com/questions/4964142/how-to-spawn-n-threads
    // https://www.geeksforgeeks.org/function-pointer-in-c/
    // https://stackoverflow.com/questions/11624545/how-to-make-main-thread-wait-for-all-child-threads-finish
    int threads = THREADS, i = 0;
    void (*funcaoDoSexo);

    printf("Iniciando a execução com %d threads.\n", threads);

    // Alocação dinâmica de um espaço de memória para n-threads
    pthread_t * thread = malloc(sizeof(pthread_t)*threads);

    for (i = 0; i < threads; i++) {

        // if (i % 2) {
        funcaoDoSexo = &entrarNoBanheiroMulher;
        // }
        if(pthread_create(&thread[i], NULL, funcaoDoSexo, NULL)) {
            printf ("Falha ao criar thread.\n");
            exit (1);
        }
    }

    // Join de todos os threads
    for (i = 0; i < threads; i++) {
        pthread_join(thread[i], NULL);
    }

    printf("Finalizando a execução.\n");

    return 0;
}
