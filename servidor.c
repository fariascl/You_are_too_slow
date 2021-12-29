/*
    ╔═══════════════════════╗ 
        PROCESO SERVIDOR
    ╚═══════════════════════╝

    ~ Alejandro Farías S.
    ~ Pablo Montoya G.

    .: Sistemas Operativos 2021-2 :.
*/

/* .: LIBRERIAS :. */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

/* .: FUNCIONES :. */

/* .: PROGRAMA PRINCIPAL :. */
int main()
{
    /* .: VARIABLES :. */
    int cantidadJugadores, i;
    pid_t servidor_Cliente1, servidor_Cliente2, servidor_Cliente3, servidor_Cliente4;

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

    /* .: CREACIÓN de PROCESOS SERVIDOR - CLIENTE :. */
    if((servidor_Cliente1 = fork())==0)
    {
        printf("Cliente 1 creado\n");
    }
    else
    {
        if((servidor_Cliente2 = fork())==0)
        {
            printf("Cliente 2 creado\n");
        }
        else
        {
            if(cantidadJugadores > 2)
            {
                if((servidor_Cliente3 = fork())==0)
                {
                    printf("Cliente 3 creado\n");
                }
                else
                {
                    if(cantidadJugadores == 4)
                    {
                        if((servidor_Cliente4 = fork())==0)
                        {
                            printf("Cliente 4 creado\n");
                        }
                    }
                }
            }
        }
    }
    
    // Creación de procesos hijos en base al numero de jugadores seleccionados.


    /*╔══════════╗ 
        CLIENTES
      ╚══════════╝ */
    if(servidor_Cliente1 == 0)
    {
        
    }
    /*╔══════════╗ 
        SERVIDOR
      ╚══════════╝ */
    else
    {
        
    }

    //? Instrucción temporal para realizar pruebas.
    while(1);

    return 0;
}

/* .: DESARROLLO DE FUNCIONES :. */