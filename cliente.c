/*
    ╔═════════════════════════════╗ 
        PROCESO CLIENTE JUGADOR    
    ╚═════════════════════════════╝

    ~ Alejandro Farías S.
    ~ Pablo Montoya G.

    .: Sistemas Operativos 2021-2 :.
*/

//* .: LIBRERIAS :. *//
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

//* .: TUBERÍA :. *//
#define FIFO "tuberia"

//* .: VARIABLES GLOBALES :. *//
siginfo_t informacionServidor;

//* .: FUNCIONES :. *//
void instruccionJuego();

//* .: PROGRAMA PRINCIPAL :. *//
int main()
{

    //* .: VARIABLES :. *//
    int fd, pid;
    char mensaje[20];

    //* .: ABRIR TUBERÍA PARA COMUNICARSE CON EL SERVIDOR :. *//
    if((fd = open(FIFO, O_RDWR)) < 0)
    {
        printf("No se pudo conectar al servidor (Servidor Cerrado)\n");
        exit(1);
    }

    // Enviar PID por la tubería para que se nos asigne un Cliente Servidor.
    pid = getpid();

    write(fd, &pid, sizeof(pid));
    write(fd, &pid, sizeof(pid));

    // Información al jugador.
    printf("Esperando a jugadores ...\n");

    //* .: DESARROLLO DEL JUEGO :. *//
    while(1)
    {

        //* .: MANEJO DE SEÑALES :. *//
        // Señales para turno.
        signal(SIGINT, &instruccionJuego);
        signal(SIGTSTP, &instruccionJuego);

        signal(SIGUSR1, &instruccionJuego);
        signal(SIGUSR2, &instruccionJuego);

        // Crear lista de señales.
        sigset_t lista_Signals;

        sigemptyset(&lista_Signals); // Limpiar variable.

        // Asignar señales de Usuario a la lista.
        sigaddset(&lista_Signals, SIGUSR1);
        sigaddset(&lista_Signals, SIGUSR2);

        // Asignar señales 2 y 20 a la lista.
        sigaddset(&lista_Signals, SIGINT);
        sigaddset(&lista_Signals, SIGTSTP);
        
        sigprocmask(SIG_BLOCK, &lista_Signals, NULL);

        system("clear");

        printf("Robar el turno: CTRL + C\n");

        // Esperar a que se reciba señal (Información) para empezar el turno.
        int signal_info = sigwaitinfo(&lista_Signals, &informacionServidor);

        //? Instrucción temporal.
        printf("Señal: %d\n", signal_info);

        // Manejo de señales
        switch (signal_info)
        {
            // Señal SIGINT (kill -l)
            case 2:

                printf("CTRL + C\n");

                break;

            // Señal SIGUSR1 (kill -l)
            case 10:
                
                break;
            
            // Señal SIGUSR2 (kill -l)
            case 12:

                break;

            // Señal SIGTSTP (kill -l)
            case 20:

                printf("CTRL + Z\n");

                break;

            default:
                // Error
                break;
        }

    }

    return 0;
}

//* .: DESARROLLO DE FUNCIONES :. *//

/* Función para tratar las señales que envíe el proceso
    CLIENTE - SERVIDOR asignado a este jugador */
void instruccionJuego()
{

}