#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>


// Fução que trata os sinais
void tratarSinal(int sinal) {

    switch (sinal) {

    case SIGUSR1:
        std::cout << "SIGUSR1" << " \n";
        tarefaUm();
        break;

    case SIGUSR2:
        std::cout << "SIGUSR2" << " \n";
        break;
    
    case SIGTERM:
        std::cout << "Finalizando o disparador...”" << " \n";
        exit(0); 
    }

};

void tarefaUm(int *comandoParaExecutar) {

    // Trecho baseado no exemplo da aula 1 do conjunto 2
    int descritorDePipe[2];
    
    // Cria o pipe
    if(pipe(descritorDePipe) < 0) {

        // A função pipe tem retorno negativo em caso de falhas
        std::cout << "Falha ao criar o pipe”" << " \n";
        exit(1); // Finaliza o programa com código 1
    }

    // Cria um processo filho
    int pid = fork ();

    if (pid < 0) {

        // A função fork tem retorno negativo em caso de falhas
        std::cout << "Falha ao criar o processo filhos" << " \n";
        exit(1); // Finaliza o programa com código 1
    }

    // Código do processo pai
    if (pid > 0) {

        // Trecho baseado de
        // https://www.ibm.com/docs/en/zos/2.1.0?topic=functions-wait-wait-child-process-end
        int *status;
        wait(status); // Espera até que o processo filho retorne

        read(descritorDePipe[0], comandoParaExecutar, 32); // Leitura do pipe

        // Fecha as pontas do pipe
        close (descritorDePipe[0]);
        close (descritorDePipe[1]);
    }

    // Código do processo filho
    else {

        // Trechos de https://www.geeksforgeeks.org/time-function-in-c/
        int tempo;
        tempo = time(NULL); // Ler segundos

        // Imprime o tempo
        std::cout << time << " \n";

        // Escreve no pipe
        write (descritorDePipe[1], &tempo, 32);

        // Fecha as pontas do pipe
        close (descritorDePipe[0]);
        close (descritorDePipe[1]);

        exit(0);
    }
    

};

int main()
{
    // Ler e imprimir o próprio PID
    int pid = getppid();

    // Variável do exercício
    int comandoParaExecutar;

    std::cout << pid << " \n";
    std::cout << "Iniciando tratador de sinais.. " << " \n";

    // Registrar função de tratamento de sinais
    // Modificado de https://www.thegeekstuff.com/2012/03/catch-signals-sample-c-code/
    if (signal(SIGUSR1, tratarSinal) == SIG_ERR)
        printf("Não foi possível capturar SIGUSR1\n");
        
    if (signal(SIGUSR2, tratarSinal) == SIG_ERR)
        printf("Não foi possível capturar SIGUSR2\n");
        
    if (signal(SIGTERM, tratarSinal) == SIG_ERR)
        printf("Não foi possível capturar SIGTERM\n");

    while (1) {
        sleep(1);
    }

    return 0;
}

