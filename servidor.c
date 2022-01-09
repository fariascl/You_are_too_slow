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
    srand(time(NULL));

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
    int i, j, DIMENSION, valoresRepartidos, valoresIngresados, turno = 0;
    bool continuarLlenado;

    // Variables para procesos.
    int fila, columna, respuestaTurno, puntajeJugador = 0;
    
    /*
        Caso 0: Jugador termina el turno.
        Caso 1: Jugador inicia el turno.
    */
    int jugador_con_el_turno = 0; //? Variable para informar que el jugador iniciar / termina el turno.

    /*
        Caso 0: Jugador bloqueado.
        Caso 1: Jugador desbloqueado.
    */
    int estado_jugador_turno = 1; //? Variable para indicar si el jugador puede jugar o no.

    int pidJugador;
    pid_t servidor_Cliente, pid;

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

    printf("\nEsperando jugadores ..\n\n");

    //* .: VARIABLES (Se necesita saber cantidad de jugadores para iniciarlas) :. *//
    int estadoServer = cantidadJugadores;
    int pidsHijos[cantidadJugadores];
    int tuberiasPIPE[cantidadJugadores*2][2];
    sem_t *semaforos = mmap(NULL, sizeof(sem_t)*cantidadJugadores, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *semaforo_Espera_Jugadores = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    // Iniciar N semáforos //
    for(i = 0; i < cantidadJugadores; i++)
    {
        sem_init(&semaforos[i], 1, 0);
    }

    sem_init(semaforo_Espera_Jugadores, 1 , 0);

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

            // Enviar PID al proceso JUGADOR.
            pid = getpid();

            write(fd, &pid, sizeof(pid));

            // Asignar numero de jugador a proceso SERVIDOR - CLIENTE.
            numeroHijo = i;

            printf("Jugador %d conectado\n", i);
            estadoServer --;

            // Liberar recurso para conectar siguiente jugador al siguiente cliente.
            sem_post(semaforo_Espera_Jugadores);

            break;
        }
        //? Código de SERVIDOR - PADRE //
        else
        {
            close(tuberiasPIPE[i*2][1]);
            close(tuberiasPIPE[(i*2)+1][0]);

            // Pausar ejecución para asignar jugadores a clientes.
            sem_wait(semaforo_Espera_Jugadores);

            // Guardar PID del hijo creado.
            pidsHijos[i] = servidor_Cliente;
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

        //? Llenar matriz con valores del arreglo
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
        
        printf("\nACTUALIZACIÓN MATRIZ REAL:\n");

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
        }

        printf("\n\n");

        printf(".: TURNO %d :.\n", turno);

        // Reanudar ejecucion de procesos SERVIDOR - CLIENTE.
        for(i = 0; i < cantidadJugadores; i++)
        {
            sem_post(&semaforos[i]);
        }
    }

    //! PRUEBA PARA PAUSAR PROGRAMA !//
    /*while(estadoServer > 0)
    {
        printf("Esperando ...\n");
        sleep(2);
    }*/

    //* .: CÓDIGO DEL JUEGO :. *//
    while(1)
    {
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

                    // Información al usuario.
                    printf("\nJugador %d roba el turno.\n", numeroHijo);
                    //printf("Puntos: %d\n\n", puntajeJugador);

                    // Reservar recurso para que el jugador ingrese sus opciones.
                    kill(getppid(), 10); // getppid -> pid del padre.

                    // Leer datos que el jugador escribió.
                    read(fd, &fila, sizeof(fila));

                    read(fd, &columna, sizeof(columna));

                    printf("\nFila: %d\nColumna: %d\n\n", fila, columna);

                    fflush(stdout);

                    //* .: VALIDAR COORDENADA ENVIADA Y ENVIAR MENSAJES :. *//

                    /*
                        respuestaTurno 1: "Coordenada errónea o ya revelada .."
                        respuestaTurno 2: "Has acertado!!"
                    */
                    if(fila < 0 || fila >= DIMENSION || columna < 0 || columna >= DIMENSION)
                    {
                        //? Enviar señal al padre para que desbloquee los procesos y termine el turno.
                        //int auxiliar = kill(getppid(), 10);
                        //printf("\nSe envio %d\n", auxiliar);
                        kill(getppid(), 10);

                        //? Enviar señal al jugador para que reciba la respuesta de su turno.
                        kill(pidJugador, 10);

                        // Enviar mensaje de coordenadas erroneas.
                        respuestaTurno = 1;

                        write(fd, &respuestaTurno, sizeof(respuestaTurno));

                        // Información para el servidor.
                        printf("Coordenada erronea\n");
                    }
                    else
                    {
                        // Enviar mensaje de coordenada ya revelada.
                        if(matrizOculta[fila][columna] == 'X')
                        {

                            //? Enviar señal al padre para que desbloquee los procesos y termine el turno.
                            kill(getppid(), 10);

                            //? Enviar señal al jugador para que reciba la respuesta de su turno.
                            kill(pidJugador, 10);

                            respuestaTurno = 1;

                            write(fd, &respuestaTurno, sizeof(respuestaTurno));
                            
                            // Información para el servidor.
                            printf("Coordenada revelada\n");

                            fflush(stdout);
                        }
                        // Enviar mensaje de exito.
                        else
                        {
                            printf("Coordenada acertada\n");

                            //? Enviar señal al padre para que espere coordenadas en tubería PIPE.
                            kill(getppid(), 12);

                            //? Enviamos coordenadas al padre.
                            
                            write(tuberiasPIPE[numeroHijo * 2][1], &fila, sizeof(int)); // Fila.
                            sem_wait(&semaforos[numeroHijo]);

                            write(tuberiasPIPE[numeroHijo * 2][1], &columna, sizeof(int)); // Columna.
                            sem_wait(&semaforos[numeroHijo]);

                            //? Recibimos respuesta del padre.
                            read(tuberiasPIPE[(numeroHijo * 2) + 1][0], &respuestaTurno, sizeof(respuestaTurno));

                            // Aumentamos el puntaje del jugador.
                            puntajeJugador += respuestaTurno;

                            //? Enviar señal al jugador para que reciba la respuesta de su turno.
                            kill(pidJugador, 10);

                            // Enviamos resultado al jugador.
                            respuestaTurno = 2;
                            write(fd, &respuestaTurno, sizeof(int));

                            fflush(stdout);

                            //? Enviar señal al padre para que desbloquee los procesos y termine el turno.
                            kill(getppid(), 10);
                        }
                        
                    }

                    // Informar a la terminal el puntaje.
                    printf("Jugador %d: %d puntos\n", numeroHijo, puntajeJugador);

                    break;
                
                // Señal SIGUSR2 (kill -l)
                case 12:

                    //* Enviar señal al jugador para bloquear / desbloquear turno *//
                    if(estado_jugador_turno == 0)
                    {
                        printf("\nJugador %d desbloqueado\n", numeroHijo);

                        estado_jugador_turno = 1;
                    }
                    else
                    {
                        printf("\nJugador %d bloqueado\n", numeroHijo);

                        estado_jugador_turno = 0;
                    }
                    
                    //printf("Jugador %d bloqueado / desbloqueado\n", numeroHijo);

                    //fflush(stdout);

                    kill(pidJugador, 12);
                    //fflush(stdout);

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
            // Variable encargada de saber cual jugador tiene el turno.
            int jugador_que_tiene_turno; //! Nombre lo más descriptivo posible jajaja.

            //* .: DESARROLLO DEL JUEGO :. *//

            // Esperar a que se reciba señal (Información) para empezar el turno.
            int signal_info = sigwaitinfo(&lista_Signals, &informacionProceso); // Informacion de la señal que envió el que robó el turno

            switch (signal_info)
            {
                // Señal SIGUSR1 (kill -l)
                case 10:

                    //? Caso 1: Bloquear procesos y asignar turno al jugador que llegó primero.
                    //? Caso 0: Desbloquear procesos una vez que el turno terminó.
                    for(i = 0; i < cantidadJugadores; i++)
                    {

                        // Consultar PID del cliente que envió la señal.
                        if(informacionProceso.si_pid == pidsHijos[i])
                        {
                            if(jugador_con_el_turno == 0)
                            {
                                printf("Jugador %d inicia\n", i);

                                jugador_con_el_turno = 1;
                            }
                            else
                            {
                                printf("Jugador %d termina\n", i);

                                jugador_con_el_turno = 0;
                                turno ++;

                                printf("\n.: TURNO %d :.\n", turno);
                            }
                            
                            //printf("Jugador %d inicia / termina\n", i);

                            // Informar que el jugador i será el que mande información.
                            jugador_que_tiene_turno = i;

                        }
                        else
                        {
                            kill(pidsHijos[i], 12);
                            printf("Jugador %d bloqueado / desbloqueado\n", i);
                        }
                        
                    }

                    break;
                
                // Señal SIGUSR2 (kill -l)
                case 12:

                    //? Proceso SERVIDOR - CLIENTE informa que enviará coordenadas.
                    read(tuberiasPIPE[jugador_que_tiene_turno * 2][0], &fila, sizeof(int)); // Fila.
                    sem_post(&semaforos[jugador_que_tiene_turno]);

                    read(tuberiasPIPE[jugador_que_tiene_turno * 2][0], &columna, sizeof(int)); // Columna.

                    // Asignar puntaje al jugador.
                    respuestaTurno = matrizReal[fila][columna];

                    // Modificamos matriz oculta.
                    matrizOculta[fila][columna] = 'X';

                    // Enviar puntaje al hijo por la tubería.
                    write(tuberiasPIPE[(jugador_que_tiene_turno * 2) + 1][1], &respuestaTurno, sizeof(int)); // Columna.

                    sem_post(&semaforos[jugador_que_tiene_turno]);

                    break;

                default:
                    // Error
                    break;
            }
        }
    }
    //? Instrucción temporal para realizar pruebas.
    //while(1);

    return 0;
}

//* .: DESARROLLO DE FUNCIONES :. *//

/* ESCRIBIR LO QUE HACE LA FUNCIÓN */
void instruccionJuego()
{

}