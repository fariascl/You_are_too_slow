/*
    PROCESO SERVIDOR

    Alejandro Farías S.
    Pablo Montoya G.

    Sistemas Operativos 2021-2
*/

/* LIBRERIAS */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

/* FUNCIONES */

//* PROGRAMA PRINCIPAL *//
int main()
{
    /* VARIABLES */
    int cantidadJugadores, i;
    pid_t svCliente;

    // Ingreso de numero de jugadores.
    do
    {
        system("clear");

        printf("Ingrese numero de jugadores: ");
        scanf("%d", &cantidadJugadores);

        if(cantidadJugadores < 2 || cantidadJugadores > 4)
        {
            printf("\nERROR! Ingrese un valor valido...\n");
            sleep(2);
        }

    } while (cantidadJugadores < 2 || cantidadJugadores > 4);

    // Creación de procesos hijos en base al numero de jugadores seleccionados.
    for(i=1; i <= cantidadJugadores; i++)
    {
        svCliente = fork();

        // Instrucción para dar paso a la creación de un nuevo hijo.
        if(svCliente == 0)
        {
            break;
        }
        // Mensaje de error.
        else if(svCliente == -1)
        {
            perror("ERROR crear proceso hijo, reinicie el programa...\n");
			exit(1);
			break;
        }

    }

    //? Instrucción temporal para realizar pruebas.
    while(1);

    return 0;
}

/* DESARROLLO DE FUNCIONES */