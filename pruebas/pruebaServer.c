/*  Código destinado a realizar pruebas con el servidor.

    Historial de pruebas:

        1) Manejar programa con solo 1 hijo [LISTO]
        2) Crear tubería para comunicar hijo con padre [LISTO] -> Buscar como enviar arreglo int a la tubería.
        3) Crear 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define N 3

/* Funciones */
int asignarPuntaje(int matriz[N][N], int fila, int columna)
{
    
}

int main()
{
    /* Variables */
    pid_t hijo;

    // (P2) Creación de la matriz.
    int matriz[3][3];
    int cont = 1, fila, columna;
    int posicion[2];
    char mensaje[3];

    for(int i=0; i<3; i++)
    {
        for(int j=0; j<3; j++)
        {
            matriz[i][j] = cont;
            cont ++;
        }
    }

    // (P2) Tubería.
    int fd[2];

    // (P2) Abrir tubería.
    pipe(fd);

    hijo = fork();

    switch (hijo)
    {
    case 0:

        //printf("Soy el hijo || PID: %d\n", getpid());
        //printf("[HIJO] Numero: %d\n", numero);

        // (P2) Cerrar Extremo Lectura de la Tubería.
        close(fd[0]);

        // (P2) Escribir en la tubería la posicion a revelar.
        printf("Ingrese fila: "); scanf("%d", &fila);
        printf("Ingrese columna: "); scanf("%d", &columna);

        /*printf("Ingrese [fila columna]: ");
        fgets(mensaje, 3, stdin);*/
        //printf("Matriz: %d\n", matriz[posicion[0]][posicion[1]]);

        write(fd[1], &fila, sizeof(int));
        write(fd[1], &columna, sizeof(int));

        //write(fd[1], mensaje, 3);

        break;
    
    default:

        //printf("Soy el padre || PID: %d\n", getpid());

        //int numero = 3;
        //printf("[PADRE] Numero: %d\n", numero);

        // (P2) Cerrar Extremo Escritura de la Tubería.
        close(fd[1]);

        // (P2) Recibir desde la tubería la posicion a revelar.
        read(fd[0], &fila, sizeof(int));
        read(fd[0], &columna, sizeof(int));
        //read(fd[0], mensaje, 3);

        //printf("Fila: %d || Columna: %d\n", posicion[0], posicion[1]);

        //printf("Valor: %d\n", matriz[posicion[0]][posicion[1]]);
        //printf("Valor: %d\n", matriz[mensaje[0]][mensaje[2]]);
        printf("Valor: %d\n", matriz[fila][columna]);

        //wait(NULL);

        break;
    }

    
    return  0;
}