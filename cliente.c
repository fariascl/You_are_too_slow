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
#include <stdbool.h>

//* .: TUBERÍA :. *//
#define FIFO "tuberia"

//* .: VARIABLES GLOBALES :. *//
siginfo_t informacionServidor;

//* .: FUNCIONES :. *//
void instruccionJuego();

//* .: PROGRAMA PRINCIPAL :. *//
int main()
{
    system("clear");

    //* .: MANEJO DE SEÑALES :. *//
    // Señales para turno.
    signal(SIGINT, &instruccionJuego);
    signal(SIGTSTP, &instruccionJuego);

    signal(SIGUSR1, &instruccionJuego);
    signal(SIGUSR2, &instruccionJuego);

    // Crear lista de señales.
    sigset_t lista_Signals;
    sigset_t activar_desactivar_cambios;

    sigemptyset(&lista_Signals); // Limpiar variable.

    // Asignar señales de Usuario a la lista.
    sigaddset(&lista_Signals, SIGUSR1);
    sigaddset(&lista_Signals, SIGUSR2);

    // Asignar señales 2 y 20 a la lista.
    sigaddset(&lista_Signals, SIGINT);
    sigaddset(&lista_Signals, SIGTSTP);

    // Activar señales (Para poder robar turno).
    sigaddset(&activar_desactivar_cambios, SIGUSR1);

    // Desactivar señales (Porque otro jugador tiene el turno).
    sigaddset(&activar_desactivar_cambios, SIGUSR2);

    // Guardar cambios.
    sigprocmask(SIG_BLOCK, &lista_Signals, NULL);
    
    //* .: VARIABLES :. *//
    int fd, pid, respuestaTurno;
    int fila, columna;
    char mensaje[20];
    bool ignorarSignal, permisos = true;
    pid_t clienteAsignado;
    sigset_t colaSignals; // Señales en cola para ser ejecutadas.

    //* .: ABRIR TUBERÍA PARA COMUNICARSE CON EL SERVIDOR :. *//
    if((fd = open(FIFO, O_RDWR)) < 0)
    {
        printf("No se pudo conectar al servidor (Servidor Cerrado)\n");
        exit(1);
    }

    // Enviar PID por la tubería para que se nos asigne un Cliente Servidor.
    pid = getpid();

    // Validación de que se conectó correctamente al servidor.
    do
    {
        
        write(fd, &pid, sizeof(pid));

        // Recibir PID del cliente asignado al Jugador.
        read(fd, &clienteAsignado, sizeof(clienteAsignado));

    }while(clienteAsignado == pid);

    // Información al jugador.
    printf("Conectado con exito al cliente %d\n\n", clienteAsignado); //! AGREGAR MARCO.
    printf("Esperando a jugadores ...\n");

    //* .: DESARROLLO DEL JUEGO :. *//
    while(1)
    {

        //* .: IGNORAR SEÑALES FUERA DE TURNO :. *//
        sigpending(&colaSignals);
        ignorarSignal = false; // No existen señales en cola.

        // Señal a ignorar #1
        if(sigismember(&colaSignals, SIGINT))
        {
            ignorarSignal = true;
        }

        // Señal a ignorar #2
        if(sigismember(&colaSignals, SIGUSR1))
        {
            ignorarSignal = true;
        }

        // Señal a ignorar #3
        if(sigismember(&colaSignals, SIGUSR2)){
            ignorarSignal = true;
        }
        
        // Señal a ignorar #4
        if(sigismember(&colaSignals, SIGTSTP))
        {
            ignorarSignal = true;
        }

        //system("clear");

        printf("Robar el turno: CTRL + C\n");

        // Esperar a que se reciba señal (Información) para empezar el turno.
        int signal_info;

        if(permisos == true)
        {
            signal_info = sigwaitinfo(&lista_Signals, &informacionServidor);
        }
        else
        {
            signal_info = sigwaitinfo(&activar_desactivar_cambios, &informacionServidor);
        }

        printf("Señal recibida: %d\n", informacionServidor.si_signo);
        printf("Info: %d\n", informacionServidor.si_pid);

        //? Instrucción temporal.
        //printf("Señal: %d\n", signal_info);

        // Manejo de señales
        switch (signal_info)
        {
            // Señal SIGINT (kill -l)
            case 2:

                // Enviar señal al proceso cliente asignado.
                kill(clienteAsignado, 10);

                // Pedir datos.
                printf("\nIngrese fila: ");
                scanf("%d", &fila);
                fflush(stdin);

                printf("Ingrese columna: ");
                scanf("%d", &columna);
                fflush(stdin);
                
                // Enviar datos a traves de la tubería.
                write(fd, &fila, sizeof(fila));
                write(fd, &columna, sizeof(columna));

                //printf("\nFila enviada: %d\nColumna enviada: %d\n\n", fila, columna);

                break;

            // Señal SIGUSR1 (kill -l)
            case 10:

                // Esperar respuesta del servidor.
                read(fd, &respuestaTurno, sizeof(respuestaTurno));

                // Imprimir mensajes.
                if(respuestaTurno == 1)
                {
                    printf("\nCoordenada errónea o ya revelada ..\n");
                }
                else
                {
                    printf("\nHas acertado!!\n");
                }
                
                break;
            
            // Señal SIGUSR2 (kill -l)
            case 12:

                // Desactivar permisos (Otro jugador robó el turno).
                if(permisos == true)
                {
                    system("clear");
                    printf("YOU ARE TOO SLOW!!\n\nEsperando turno de rival...\n\n");
                    sigprocmask(SIG_BLOCK, &activar_desactivar_cambios, NULL);
                    permisos = false;
                }
                // Activar permisos (Reinicio de turno).
                else
                {
                    system("clear");
                    printf("Preparate para el nuevo turno\n\n");
                    fflush(stdout);
                    sigprocmask(SIG_BLOCK, &lista_Signals, NULL);
                    permisos = true;
                }

                break;

            // Señal SIGTSTP (kill -l)
            case 20:

                printf("CTRL + Z\n");

                break;

            default:
                // Error
                break;
        }

        sleep(2);

    }

    return 0;
}

//* .: DESARROLLO DE FUNCIONES :. *//

/* Función para tratar las señales que envíe el proceso
    CLIENTE - SERVIDOR asignado a este jugador */
void instruccionJuego()
{

}