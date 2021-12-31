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
#include <pthread.h>
#include <semaphore.h>

/* .: TUBERÍA :. */
#define FIFO "tuberia"

/* .: FUNCIONES :. */
//void llenarMatrizOculta(int filas, int columnas);

/* .: PROGRAMA PRINCIPAL :. */
int main()
{
    /* .: VARIABLES :. */
    int cantidadJugadores, i, fd, DIMENSION;
    pid_t servidor_Cliente, pidJugador;

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

    /* .: VARIABLES (Se necesita saber cantidad de jugadores para iniciarlas) :. */
    int tuberiasPIPE[cantidadJugadores*2][2];
    sem_t semaforos[cantidadJugadores];

    /* .: CREACIÓN DE TUBERÍA FIFO PARA COMUNICACIÓN CON SERVIDOR - JUGADOR :. */
    unlink(FIFO);

    // Crear tubería.
    if(mkfifo(FIFO, 0666) < 0)
    {
        perror("mkfifo");
        exit(1);
    }

    // Abrir tubería.
    if((fd = open(FIFO, O_RDWR)) < 0)
    {
        perror("open");
        exit(1);
    }

    /* .: CREACIÓN de MATRIZ COMPARTIDA PARA PROCESOS SERVIDOR - CLIENTE :. */
    if(cantidadJugadores == 2)
    {
        DIMENSION = 8;
    }
    
    if(cantidadJugadores == 3)
    {
        DIMENSION = 10;
    }

    if(cantidadJugadores == 4)
    {
        DIMENSION = 12;
    }

    int matrizOculta[DIMENSION][DIMENSION];

    /* .: CREACIÓN de PROCESOS SERVIDOR - CLIENTE :. */
    for(int i = 1; i <= cantidadJugadores; i++)
    {

        //* Iniciamos semáforo por posición *//
        sem_init(&semaforos[i], 0, 0);

        //* Creamos tubería para que SERVIDOR - CLIENTE pueda escribir *//
        if(pipe(tuberiasPIPE[i*2]) < 0)
        {
            printf("ERROR!\n");
            exit(1);
        }

        //* Creamos tubería para que SERVIDOR - CLIENTE pueda leer *//
        if(pipe(tuberiasPIPE[(i*2)+1]) < 0)
        {
            printf("ERROR!\n");
            exit(1);
        }

        //* Código de SERVIDOR - CLIENTE *//
        if((servidor_Cliente = fork()) == 0)
        {
            close(tuberiasPIPE[i*2][0]);
            close(tuberiasPIPE[(i*2)+1][1]);

            read(fd, &pidJugador, sizeof(pidJugador));
            printf("Jugador %d conectado\n", i);
            break;
        }
        //* Código de SERVIDOR - PADRE *//
        else
        {
            close(tuberiasPIPE[i*2][1]);
            close(tuberiasPIPE[(i*2)+1][0]);

            read(fd, &pidJugador, sizeof(pidJugador));
        }

    }

    /*╔══════════╗ 
        CLIENTES
      ╚══════════╝ */
    if(servidor_Cliente == 0)
    {
        
    }
    /*╔══════════╗ 
        SERVIDOR
      ╚══════════╝ */
    else
    {
        /* .: CREACIÓN de MATRIZ PARA PROCESO SERVIDOR - PADRE :. */
        if(cantidadJugadores == 2)
        {
            DIMENSION = 8;
        }
        
        if(cantidadJugadores == 3)
        {
            DIMENSION = 10;
        }

        if(cantidadJugadores == 4)
        {
            DIMENSION = 12;
        }

        int matrizReal[DIMENSION][DIMENSION];
    }

    //? Instrucción temporal para realizar pruebas.
    while(1);

    return 0;
}

/* .: DESARROLLO DE FUNCIONES :. */
