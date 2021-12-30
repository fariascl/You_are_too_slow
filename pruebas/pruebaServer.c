/*  Código destinado a realizar pruebas con el servidor.

    Historial de pruebas:

        1) Manejar programa con solo 1 hijo [LISTO]
        2) Crear tubería para comunicar hijo con padre [LISTO] -> Buscar como enviar arreglo int a la tubería.
        3) Conectar proceso servidor con jugador mediante tubería [LISTO]
        4) Conectar tubería teniendo varios hijos [LISTO] -> No queda claro el funcionamiento total,
                                                                hay que hacer más pruebas.
        5) Conectar jugador a hijo y bloquear acceso a la memoria mediante semáforo [PENDIENTE]
        6) Lograr que hijo reconozca pid del jugador que se conecta [LISTO]
        7) Usar funciones de hilos para ejecutar código de cada hijo []
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>

// Nombre de la tubería.
#define FIFO "tuberia"
#define N 3
// Nombre del semáforo.
int estadoServidor, pidJugador, jugadorAsignado1, jugadorAsignado2, fd;
sem_t semaforo1, semaforo2;

/* Funciones */
void instruccionesHijo1();
void instruccionesHijo2();

int main()
{
    /* Variables */
    pid_t hijo1, hijo2;
    pthread_t hilo1, hilo2;

    printf("PADRE %d\n\n", getpid());

    // (P3) Variable que almacenará la matriz.
    int M;

    // (P5) Indicar que el servidor está disponible para recibir jugadores.
    estadoServidor = 1;

    // (P3) Cerrar tubería.
    unlink(FIFO);

    // (P3) Crear tubería.
    if(mkfifo(FIFO, 0666) < 0)
    {
        perror("mkfifo");
        exit(1);
    }

    // (P3) Abrir tubería.
    if((fd = open(FIFO, O_RDWR)) < 0)
    {
        perror("open");
        exit(1);
    }
    

    for(int i = 1; i < 3; i++)
    {
        if((hijo1 = fork()) == 0)
        {
            
            read(fd, &pidJugador, sizeof(pidJugador));
            printf("Jugador conectado a hijo %d\n", i);
            break;
        }
        else
        {
            read(fd, &pidJugador, sizeof(pidJugador));
        }

    }

    if(hijo1 == 0)
    {

    }
    else
    {
        printf("\n\n(%d) TODOS LOS JUGADORES CONECTADOS\n\n", getpid());
    }
    
    // (P5) Inicializar semaforo.
    sem_init(&semaforo1, 0, 1);
    sem_init(&semaforo2, 0, 1);

    // (P7) Creamos 2 hijos para realizar pruebas.
    /*hijo1 = fork();

    printf("Esperando jugadores...\n");

    if(hijo1 == 0)
    {
        // (P5) Abrimos hilo para asignar jugador conectado al servidor.
        printf("Hijo 1 creado\n");

        jugadorAsignado1 = 0;
        char mensaje[20];

        while(estadoServidor > 0)
        {
            
            if(jugadorAsignado1 == 0)
            {
                sem_wait(&semaforo1);
                printf("\n\nESTADO SERVER %d\n\n", estadoServidor);
                read(fd, &pidJugador, sizeof(pidJugador));
                printf("El jugador %d se conecto\n", pidJugador);
                printf("Pasaste por el hijo1\n");
                jugadorAsignado1 = pidJugador;
                estadoServidor --;
                sem_post(&semaforo1);

                printf("Faltan %d jugadores\n", estadoServidor);
            }

        }
        
        printf("JUGADOR ASIGNADO AL HIJO 1 -> %d\n", jugadorAsignado1);
        printf("Estado servidor: %d\n", estadoServidor);

        sem_wait(&semaforo1);

        read(fd, &mensaje, sizeof(mensaje));
        printf("Jugador1 dice: %s\n", mensaje);
            
        sem_post(&semaforo1);
    }
    else
    {
        // (P7) Creamos segundo hijo.
        if((hijo2 = fork()) == 0)
        {
            printf("Hijo 2 creado\n");
            char mensaje[20];
            int M;

            while(estadoServidor > 0)
            {

                if(jugadorAsignado2 == 0)
                {
                    sem_wait(&semaforo2);
                    
                    printf("\n\nESTADO SERVER %d\n\n", estadoServidor);

                    read(fd, &pidJugador, sizeof(pidJugador));
                    printf("El jugador %d se conecto\n", pidJugador);
                    printf("Pasaste por el hijo2\n");
                    jugadorAsignado2 = pidJugador;
                    estadoServidor --;
                    sem_post(&semaforo2);

                    printf("Faltan %d jugadores\n", estadoServidor);
                }

            }

            printf("JUGADOR ASIGNADO AL HIJO 2 -> %d\n", jugadorAsignado2);
            printf("Estado servidor: %d\n", estadoServidor);

            sem_wait(&semaforo2);

            read(fd, &mensaje, sizeof(mensaje));
            printf("Jugador2 dice: %s\n", mensaje);
            
            sem_post(&semaforo2);
        }

    }

    // (P2) Creación de la matriz.
    /*int matriz[3][3];
    int cont = 1, fila, columna;
    int posicion[2];*/
    //char mensaje[20];

    /*for(int i=0; i<3; i++)
    {
        for(int j=0; j<3; j++)
        {
            matriz[i][j] = cont;
            cont ++;
        }
    }*/

    // (P2) Tubería.
    //int fd[2];

    // (P2) Abrir tubería.
    //pipe(fd);

    // (P4) Crear varios hijos
    //int cantidadJugadores = 2, i;

    /*for(i=1; i <= cantidadJugadores; i++)
    {
        hijo = fork();

        // Instrucción para dar paso a la creación de un nuevo hijo.
        if(hijo == 0)
        {
            break;
        }
        // Mensaje de error.
        else if(hijo == -1)
        {
            perror("ERROR crear proceso hijo, reinicie el programa...\n");
			exit(1);
			break;
        }
    }
    */

    /*hijo = fork();

    switch (hijo)
    {
    case 0:
        
        printf("Soy el hijo || PID: %d\n", getpid());*/
        //printf("[HIJO] Numero: %d\n", numero);

        // (P2) Cerrar Extremo Lectura de la Tubería.
        //close(fd[0]);

        // (P2) Escribir en la tubería la posicion a revelar.
        //printf("Ingrese fila: "); scanf("%d", &fila);
        //printf("Ingrese columna: "); scanf("%d", &columna);

        /*printf("Ingrese [fila columna]: ");
        fgets(mensaje, 3, stdin);*/
        //printf("Matriz: %d\n", matriz[posicion[0]][posicion[1]]);

        //write(fd[1], &fila, sizeof(int));
        //write(fd[1], &columna, sizeof(int));

        //write(fd[1], mensaje, 3);

        // (P5) Identificar cuando la tubería está cerrada.
        /*printf("ESTADO TUBERIA: %d\n", fd);

        jugadorAsignado = pidJugador;

        // Leer información contenida en la tubería.
        while(jugadorAsignado > 0)
        {
            
            M = read(fd, mensaje, 20);

            // Imprime lo leído en la tubería.
            printf("Jugador %d dice: ", jugadorAsignado);
            write(1, mensaje, M);
            printf("ESTADO TUBERIA: %d\n", fd);

        }*/

        //break;
    
    //default:

        //printf("Soy el padre || PID: %d\n", getpid());

        //int numero = 3;
        //printf("[PADRE] Numero: %d\n", numero);

        // (P2) Cerrar Extremo Escritura de la Tubería.
        //close(fd[1]);

        // (P2) Recibir desde la tubería la posicion a revelar.
        //read(fd[0], &fila, sizeof(int));
        //read(fd[0], &columna, sizeof(int));
        //read(fd[0], mensaje, 3);

        //printf("Fila: %d || Columna: %d\n", posicion[0], posicion[1]);

        //printf("Valor: %d\n", matriz[posicion[0]][posicion[1]]);
        //printf("Valor: %d\n", matriz[mensaje[0]][mensaje[2]]);
        //printf("Valor: %d\n", matriz[fila][columna]);

        /*wait(NULL);

        break;
    }*/

    while(1);

    return  0;
}

