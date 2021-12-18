#include <iostream>
#include <signal.h>
#include <unistd.h>


int main()
{
    // Ler e imprimir o próprio PID
    int pid = getppid();
    std::cout << pid << " \n";
    std::cout << "Iniciando.. " << " \n";


    while (1) {
        sleep(1);
    }

}
