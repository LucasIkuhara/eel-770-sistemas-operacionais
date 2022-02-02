#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


/*
    O problema escolhido for: The unisex bathroom problem (Seção 6.2).

    Os threads são criados de maneira que seus sexos são alternados. (Um mulher, um homem)
    O programa foi desenvolvido e testado no sistema operacional Ubuntu 20.04 rodando no WSL2

    A função de usar o banheiro (usarOBanheiro), caso a macro ESPERAR esteja definida como 1,
    espera um tempo entre 0.5 e 2 segundos. Como isso não era um requisito do exercício, deixei desligado,
    mas facilitou muito a deputação de problemas de concorrência).

    A macro THREADS determina a quantidade de threads que serão usados,
    enquanto a macro MIN_USOS determina a quantidade minima de usos do banheiro para que o programa acabe.

    A macro DEBUG com o valor 1 habilita prints adicionais, também usados para depuração. Foram deixados ligados,
    pois, mesmo não sendo requisito do exercício, é bem mais fácil entender o funcionamento com eles.
    A quantidade de threads utilizada pode ser definida pela macro abaixo:
*/
#define THREADS 10
#define MIN_USOS 100
#define ESPERAR 0
#define DEBUG 1

// Variáveis de condição para o sexo oposto entrar no banheiro
pthread_cond_t  sexoCond = PTHREAD_COND_INITIALIZER;

// Variável de condição para banheiro cheio
pthread_cond_t  cheioCond = PTHREAD_COND_INITIALIZER;

// Armazena a quantidade de pessoas no banheiro em um dado momento
int qtdNoBanheiro = 0;

// Armazena sexo usando o banheiro (0 indica mulher, 1 indica homem, 2 indica que está vazio)
int sexoDoUsuario = 2;

// Contador de usos do banheiro para condição de parada
int usosDoBanheiro = 0;

// Lock usada para restringir o acesso a variável contador
pthread_mutex_t contadorLock = PTHREAD_MUTEX_INITIALIZER;

// Lock que garante o uso exclusivo da variável sexoDoUsuario
pthread_mutex_t sexoLock = PTHREAD_MUTEX_INITIALIZER;

// Lock que garante que os prints não saiam embaralhados.
// E que o acesso da variável contadora de iterações para a condição de paradas seja exclusivo
pthread_mutex_t iteradorLock = PTHREAD_MUTEX_INITIALIZER;


// Função que representa o uso do banheiro
void usarOBanheiro(){
    if(ESPERAR==1) {
        // Espera entre 0.5 e 2 segundos
        sleep(0.5 + ((float)rand()/(float)(RAND_MAX)) * 1.5);
    }
}

// Função usada para representar a entrada no banheiro
void entrarNoBanheiro(int sexo) {
    
    // O lock é para garantir que o textos dos prints não saia embaralhado,
    // e para proteger o contador de usos
    pthread_mutex_lock(&iteradorLock);

    // Imprime o uso do banheiro
    if (sexo == 0) {
        printf("[Mulher]: ");
    }
    else {
        printf("[Homem]: ");
    }
    printf("Usando o banheiro.\n");

    usosDoBanheiro++;
    
    // Caso a condição de parada seja atendida, avisar o fim, e fechar o programa.
    if (usosDoBanheiro >= MIN_USOS)
    {
        printf("O programa atingiu a condição de parada. (%d usos do banheiro)\n", usosDoBanheiro);
        exit(0);
    }
    
    pthread_mutex_unlock(&iteradorLock);

    // Espera entre 0.5s e 2s 
    // Importante: essa é a parte do código que pode de fato rodar ao mesmo tempo, visto que os prints estavam saindo
    // embaralhados, e por isso um lock. Mas em um caso real, essa seria a parte que existe concorrência enquanto a tarefa
    // mais pesada roda, que foi simulada por uma chamada sleep.
    usarOBanheiro();
   
};

// Basicamente um decorador da função printf para identificarmos o sexo de quem está falando
void printComId(char *msg, int sexo) {

    pthread_mutex_lock(&iteradorLock);

    // Só usado para prints opcionais
    if(DEBUG==1) {

        if (sexo == 0) {
            printf("[Mulher]: ");
        }
        else {
            printf("[Homem]: ");
        }

        printf(msg);
    }

    pthread_mutex_unlock(&iteradorLock);

};

void printComIdEArg(char *msg, int arg ,int sexo) {

    pthread_mutex_lock(&iteradorLock);

    // Só usado para prints opcionais
    if(DEBUG==1) {

        if (sexo == 0) {
            printf("[Mulher]: ");
        }
        else {
            printf("[Homem]: ");
        }

        printf(msg, arg);
    }

    pthread_mutex_unlock(&iteradorLock);

};


// Função que representa entrar na fila banheiro para threads
// ou seja, esperar até que as condições do problema permitam sua entrada.
void entrarNoFilaDoBanheiro(int sexo) {
        
    // Manter os threads indo para sempre ao banheiro
    while(1) {

        // Garantir o uso exclusivo da variável de sexo do usuário
        pthread_mutex_lock(&sexoLock);

        while (sexoDoUsuario != sexo && sexoDoUsuario != 2) {

            // Espera até que nenhuma pessoa do sexo oposto esteja no banheiro
            printComId("Esperando não pessoas do sexo oposto no banheiro.\n", sexo);
            pthread_cond_wait(&sexoCond, &sexoLock);
            
        }
        
        // Ativa o lock, para garantir que a variável
        // reflete a presença de um Mulher no banheiro e que foi alterada de modo exclusivo
        
        sexoDoUsuario = sexo; 

        // Libera o lock para desativar o uso exclusivo da variável sexoDoUsuario.
        pthread_mutex_unlock(&sexoLock);

        // Travar o lock para proteger a variável de contador checada no while
        pthread_mutex_lock(&contadorLock);

        // Threads esperam enquanto o banheiro está cheio (3 ou mais pessoas)
        while(qtdNoBanheiro >= 3) {
            printComId("Esperando o banheiro não estar cheio.\n", sexo);
            pthread_cond_wait(&cheioCond, &contadorLock);
        }
            
            // O lock não é liberado aqui, pois esperamos o contador ser aumentado, evitando desbloquear e
            // bloquear o lock atoa.

            // Incrementa o contador, tendo acesso exclusivo a variável contador      
            qtdNoBanheiro++;
            printComIdEArg("Entrando no banheiro. Agora tem %d pessoas no banheiro.\n", qtdNoBanheiro, sexo);

            // O mutex do contador é desbloqueado para que mais de uma pessoa consiga de fato
            // usar o banheiro por vez (Ou seja a função entrarNoBanheiro pode ser chamada por mais de
            // um thread que já tenha acesso a zona crítica)
            pthread_mutex_unlock(&contadorLock);

            // Função que representa o uso do banheiro
            entrarNoBanheiro(sexo);
            
            // Decrementa o contador de pessoas no banheiro
            pthread_mutex_lock(&contadorLock);
        
            qtdNoBanheiro--;
            printComIdEArg("Saindo do banheiro. Agora tem %d pessoas no banheiro.\n", qtdNoBanheiro, sexo);
        
            pthread_mutex_unlock(&contadorLock);

            // Se ninguém estiver no banheiro, muda o indicador de sexo para 2 (vazio)
            if (qtdNoBanheiro == 0) {
                pthread_mutex_lock(&sexoLock);
                sexoDoUsuario = 2;
                pthread_mutex_unlock(&sexoLock);
            }

            // Signal é chamado para acordar um thread esperando que o último membro do sexo oposto deixe o banheiro
            pthread_cond_signal(&sexoCond);

            // Signal é chamado para acordar um thread esperando vaga no banheiro
            pthread_cond_signal(&cheioCond);

    }       
};


int main(void) {
    
    // Referências:
    // https://stackoverflow.com/questions/4964142/how-to-spawn-n-threads
    // https://www.geeksforgeeks.org/function-pointer-in-c/
    // https://stackoverflow.com/questions/11624545/how-to-make-main-thread-wait-for-all-child-threads-finish
    // http://www.cse.cuhk.edu.hk/~ericlo/teaching/os/lab/9-PThread/Pass.html
    int threads = THREADS, i = 0;
    void (*funcaoDoSexo);

    printf("Iniciando a execução com %d threads.\n", threads);

    // Alocação dinâmica de um espaço de memória para n-threads
    pthread_t * thread = malloc(sizeof(pthread_t)*threads);

    // Define o sexo do thread criado
    int sexoDoThread;

    for (i = 0; i < threads; i++) {

        // Define o sexo pela operação módulo 2 com o contador de iteração como argumento
        sexoDoThread = i % 2;
        
        // Cria os threads
        if(pthread_create(&thread[i], NULL, &entrarNoFilaDoBanheiro, sexoDoThread)) {
            printf ("Falha ao criar thread.\n");
            exit (1);
        }
    }

    // Join de todos os threads
    for (i = 0; i < threads; i++) {
        pthread_join(thread[i], NULL);
    }

    return 0;
}
