#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>  
#include <errno.h> 
#include <iostream>

// Observação: todos os prints adicionais, ou seja, que não foram explícitamente pedidos no
// exercício foram comentados, descontando os de tratamento de erro
// caso a correção seja feita capturando a saída.

// Variável do exercício
long comandoParaExecutar = 0;


// Cria processo filho
int criarFilho() {

    // Cria um processo filho
    int pid = fork ();

    if (pid < 0) {

        // A função fork tem retorno negativo em caso de falhas
        std::cout << "Falha ao criar o processo filhos" << " \n";
        exit(1); // Finaliza o programa com código 1
    }
    
    return pid;
};


// Realiza um fork e retorna o array de descritores 
void criarPipe(int descritorDePipe[]) {

    // Trecho baseado no exemplo da aula 1 do conjunto 2 
    // Cria o pipe
    if(pipe(descritorDePipe) < 0) {

        // A função pipe tem retorno negativo em caso de falhas
        std::cout << "Falha ao criar o pipe" << " \n";
        exit(1); // Finaliza o programa com código 1
    }

    //printf("Pipe criado.")";
}


// Fecha as pontas do pipe
void fecharPipes(int* descritorDePipe) {

    close (descritorDePipe[0]);
    close (descritorDePipe[1]);
}


// Executar ping 8.8.8.8 -c 5
void executarPing() {

    // Referência de exec
    // https://linuxhint.com/exec_linux_system_call_c/

    char *programName = "ping";
    char *args[] = {programName, "8.8.8.8", "-c", "5", NULL};

    execvp(programName, args);
}

void tarefaUm() {

   // int* descritorDePipe;
    int descritorDePipe[2];
    criarPipe(descritorDePipe);

    // Cria processo filho
    int pid = criarFilho();

    // Código do processo pai
    if (pid > 0) {

        // Trecho baseado de
        // https://www.ibm.com/docs/en/zos/2.1.0?topic=functions-wait-wait-child-process-end
        wait(NULL); // Espera até que o processo filho retorne

        read(descritorDePipe[0], &comandoParaExecutar, 32); // Leitura do pipe

        // Fecha as pontas do pipe
        fecharPipes(descritorDePipe);
    }

    // Código do processo filho
    else {

        // Trechos de https://www.geeksforgeeks.org/time-function-in-c/
        //printf("lendo tempo.. ")";
        
        time_t tempo;
        // Ler segundos
        tempo = time(NULL);

        // Imprime o tempo
        // std::cout << "Tempo: " << " \n";
        std::cout << tempo << " \n";

        // Escreve no pipe
        // Como time_t é um long int
        write (descritorDePipe[1], &tempo, sizeof(tempo));

        // Fecha as pontas do pipe
        fecharPipes(descritorDePipe);

        exit(0);
    }

};


void tarefaDois() {

    //printf("Comando para executar: " << comandoParaExecutar)n";

    // Cria processo filho
    int pid = criarFilho();

    // Código do processo pai
    if (pid > 0) {

        // Espera até que o processo filho retorne
        wait(NULL); 
    }

    // Código do processo filho
    else {

        // Caso o comando seja zero
        if (comandoParaExecutar == 0) {
            std::cout << "Não há comando a executar" << " \n";

            // Pela definição do exercício, não tive certeza se o programa deveria finalizar
            // sua execução, ou a voltar a ler entradas. Assumi a segunda opção.
            return; 
        }

        else {
    
            // Caso o comando seja par
            if (comandoParaExecutar % 2 == 0) {
                printf("Par");

                // Rodar ping 8.8.8.8 -c 5
                executarPing();
            }

            // Caso o comando seja ímpar
            else {
                printf("impar");
  
                // Referência de dup2
                // http://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html
                // Duplicar STDOUT e STDERR para o file descriptor do arquivo de log
                int saida = open("saidaComando.log", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

                // Redirecionar STDIN e STDERR para saidaComando.log
                dup2(saida, STDOUT_FILENO);
                dup2(saida, STDERR_FILENO);

                // Rodar ping 8.8.8.8 -c 5
                executarPing();

            }
        }

    }


}


// Fução que trata os sinais
void tratarSinal(int sinal) {

    // Selecionar ação baseada no sinal
    switch (sinal) {

        case SIGUSR1:
            tarefaUm();
            break;

        case SIGUSR2:
            tarefaDois();
            break;

        case SIGTERM:
            std::cout << "Finalizando o disparador..." << " \n";
            exit(0); 
    }

};


int main(void)
{
    // Ler e imprimir o próprio PID
    int pid = getpid();
    std::cout << pid << "\n";
    
    // printf("Iniciando tratador de sinais.. ");

    // Registrar função de tratamento de sinais
    // Modificado de https://www.thegeekstuff.com/2012/03/catch-signals-sample-c-code/
    if (signal(SIGUSR1, tratarSinal) == SIG_ERR)
       printf("Não foi possível capturar SIGUSR1");
        
    if (signal(SIGUSR2, tratarSinal) == SIG_ERR)
       printf("Não foi possível capturar SIGUSR2");
        
    if (signal(SIGTERM, tratarSinal) == SIG_ERR)
       printf("Não foi possível capturar SIGTERM");

    while (1) {
        sleep(1);
    }

    return 0;
}
