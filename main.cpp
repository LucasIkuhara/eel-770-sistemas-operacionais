#include <iostream>
#include <signal.h>
#include <unistd.h>


// Fução que trata os sinais
void tratarSinal(int sinal) {

    switch (sinal) {

    case SIGUSR1:
        std::cout << "SIGUSR1" << " \n";
        break;

    case SIGUSR2:
        std::cout << "SIGUSR2" << " \n";
        break;
    
    case SIGTERM:
        std::cout << "Finalizando o disparador...”" << " \n";
        exit(0); 
    }

};

int main()
{
    // Ler e imprimir o próprio PID
    int pid = getppid();
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

