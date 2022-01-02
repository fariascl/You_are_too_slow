/*
    ╔═══════════════════════╗ 
        PROCESO SERVIDOR
    ╚═══════════════════════╝

    ~ Alejandro Farías S.
    ~ Pablo Montoya G.

    .: Sistemas Operativos 2021-2 :.
*/

//* .: LIBRERIAS :. *//
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

//* .: TUBERÍA :. *//
#define FIFO "tuberia"

//* .: VARIABLES GLOBALES :. *//
siginfo_t informacionProceso;

//* .: FUNCIONES :. *//
//void llenarMatrizOculta(int filas, int columnas);
void instruccionJuego();

//* .: PROGRAMA PRINCIPAL :. *//
int main()
{
    //* .: MANEJO DE SEÑALES :. *//
    signal(SIGUSR1, &instruccionJuego);
    signal(SIGUSR2, &instruccionJuego);

    sigset_t lista_Signals;

    sigemptyset(&lista_Signals); // Limpiar variable.
    sigaddset(&lista_Signals, SIGUSR1);
    sigaddset(&lista_Signals, SIGUSR2);
    sigprocmask(SIG_BLOCK, &lista_Signals, NULL);

    //* .: VARIABLES :. *//
    int cantidadJugadores, fd;

    // Variables para procesos SERVIDOR - CLIENTE.
    int numeroHijo;

    // Variables para desarrollar Matriz Real.
    int i, j, DIMENSION, valoresRepartidos, valoresIngresados;
    bool continuarLlenado;

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

    //* .: VARIABLES (Se necesita saber cantidad de jugadores para iniciarlas) :. *//
    int estadoServer = cantidadJugadores;
    int tuberiasPIPE[cantidadJugadores*2][2];
    sem_t *semaforos = mmap(NULL, sizeof(sem_t)*cantidadJugadores, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // Iniciar N semáforos //
    for(i = 0; i < cantidadJugadores; i++)
    {
        sem_init(&semaforos[i], 1, 0);
    }

    //* .: CREACIÓN DE TUBERÍA FIFO PARA COMUNICACIÓN CON SERVIDOR - JUGADOR :. *//
    unlink(FIFO);

    // Crear tubería //
    if(mkfifo(FIFO, 0666) < 0)
    {
        perror("mkfifo");
        exit(1);
    }

    // Abrir tubería //
    if((fd = open(FIFO, O_RDWR)) < 0)
    {
        perror("open");
        exit(1);
    }

    //* .: CREACIÓN de MATRIZ COMPARTIDA PARA PROCESOS SERVIDOR - CLIENTE :. *//

    // Asignar dimensión de las matrices //
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

    // Declaración de matriz oculta //
    char (*matrizOculta)[DIMENSION] = mmap(NULL, sizeof(char)*DIMENSION*DIMENSION, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    for(i = 0; i < DIMENSION; i++)
    {
        for(j = 0; j < DIMENSION; j++)
        {
            matrizOculta[i][j] = 'O'; // Posición disponible 'O' | Posición revelada 'X'
        }
    }

    //* .: CREACIÓN de PROCESOS SERVIDOR - CLIENTE :. *//
    for(int i = 0; i < cantidadJugadores; i++)
    {

        // Creamos tubería para que SERVIDOR - CLIENTE pueda escribir //
        if(pipe(tuberiasPIPE[i*2]) < 0)
        {
            printf("ERROR!\n");
            exit(1);
        }

        // Creamos tubería para que SERVIDOR - CLIENTE pueda leer //
        if(pipe(tuberiasPIPE[(i*2)+1]) < 0)
        {
            printf("ERROR!\n");
            exit(1);
        }

        //? Código de SERVIDOR - CLIENTE //
        if((servidor_Cliente = fork()) == 0)
        {
            // Cerrar extremos de las tuberías.
            close(tuberiasPIPE[i*2][0]);
            close(tuberiasPIPE[(i*2)+1][1]);

            // Recibir PID de proceso JUGADOR.
            read(fd, &pidJugador, sizeof(pidJugador));

            // Asignar numero de jugador a proceso SERVIDOR - CLIENTE.
            numeroHijo = i;

            printf("Jugador %d conectado\n", i);
            estadoServer --;
            break;
        }
        //? Código de SERVIDOR - PADRE //
        else
        {
            close(tuberiasPIPE[i*2][1]);
            close(tuberiasPIPE[(i*2)+1][0]);

            read(fd, &pidJugador, sizeof(pidJugador));
        }

    }

    //* .: ESPERAR A QUE SE CONECTEN JUGADORES :. *//
    if(servidor_Cliente == 0)
    {
        // Pausar ejecución de proceso SERVIDOR - CLIENTE para esperar nuevos jugadores.
        sem_wait(&semaforos[numeroHijo]);
    }
    else
    {
        // Reanudar ejecucion de procesos SERVIDOR - CLIENTE.
        for(i = 0; i < cantidadJugadores; i++)
        {
            sem_post(&semaforos[i]);
        }
    }

    //! PRUEBA PARA PAUSAR PROGRAMA //
    /*while(estadoServer > 0)
    {
        printf("Esperando ...\n");
        sleep(2);
    }*/

    //* .: CÓDIGO DEL JUEGO :. *//

    /*╔══════════╗ 
        CLIENTES
      ╚══════════╝ */
    if(servidor_Cliente == 0)
    {
        printf("Jugador %d: Comienza el juego!!\n", getpid());

        // Esperar a que se reciba señal (Información) para empezar el turno.
        int signal_info = sigwaitinfo(&lista_Signals, &informacionProceso); // Informacion de la señal que envió el que robó el turno

        switch (signal_info)
        {
            // Señal SIGUSR1 (kill -l)
            case 10:
                
                break;
            
            // Señal SIGUSR2 (kill -l)
            case 12:

                break;

            default:
                // Error
                break;
        }

    }
    /*╔══════════╗ 
        SERVIDOR
      ╚══════════╝ */
    else
    {
        //* .: CREACIÓN de MATRIZ PARA PROCESO SERVIDOR - PADRE :. *//
        if(cantidadJugadores == 2)
        {
            valoresRepartidos = 16;
        }
        
        if(cantidadJugadores == 3)
        {
            valoresRepartidos = 25;
        }

        if(cantidadJugadores == 4)
        {
            valoresRepartidos = 36;
        }

        // Se declara Matriz Real y arreglo con valores repartidos en la matriz.
        int matrizReal[DIMENSION][DIMENSION];
        int valoresMatriz[valoresRepartidos];

        //? Crear arreglo con N valores repartidos no repetidos.
        for(i = 0; i < valoresRepartidos; i++)
        {

            do
            {
                continuarLlenado = true;

                valoresMatriz[i] = (rand()%50)+1;

                for(j = 0; j < i; j++)
                {
                    if(valoresMatriz[i] == valoresMatriz[j])
                    {
                        continuarLlenado = false;
                    }
                }

            } while (continuarLlenado == false);

        }

        // Llenar matriz con valores del arreglo //
        do
        {
            valoresIngresados = valoresRepartidos-1;

            for(i = 0; i < DIMENSION; i++)
            {
                for(j = 0; j < DIMENSION; j++)
                {

                    matrizReal[i][j] = rand()%2;

                    if(matrizReal[i][j] == 1 && valoresIngresados >= 0)
                    {
                        matrizReal[i][j] = valoresMatriz[valoresIngresados];
                        valoresIngresados --;
                    }
                    else if(matrizReal[i][j] == 1 && valoresIngresados < 0)
                    {
                        matrizReal[i][j] = 0;
                    }

                }
            }

        } while (valoresIngresados >= 0);

        /*printf("HOLA SOY EL PADRE, MIRA LA MATRIZ:\n");

        for(i = 0; i < DIMENSION; i++)
        {
            for(j = 0; j < DIMENSION; j++)
            {

                if(matrizReal[i][j] > 9)
                {
                    printf("[ %d ]", matrizReal[i][j]);
                }
                else
                {
                    printf("[ %d  ]", matrizReal[i][j]);
                }

            }

            printf("\n");
        }*/

        //printf("PADRE: Esta todo listo\n");

        //* .: DESARROLLO DEL JUEGO :. *//

        // Esperar a que se reciba señal (Información) para empezar el turno.
        int signal_info = sigwaitinfo(&lista_Signals, &informacionProceso); // Informacion de la señal que envió el que robó el turno

        switch (signal_info)
        {
            // Señal SIGUSR1 (kill -l)
            case 10:
                
                break;
            
            // Señal SIGUSR2 (kill -l)
            case 12:

                break;

            default:
                // Error
                break;
        }
    }

    //? Instrucción temporal para realizar pruebas.
    while(1);

    return 0;
}

//* .: DESARROLLO DE FUNCIONES :. *//

/* ESCRIBIR LO QUE HACE LA FUNCIÓN */
void instruccionJuego()
{

}