/*  Código destinado a realizar pruebas varias.

    Historial de pruebas:

        1) Crear N hijos evitando que se transformen en procesos Zombie. [LISTO]
        2) Crear Matriz NxN con valores entre 1 y 50 no repetidos elegidos al azar
                dependiendo de la dimensión de la matriz, dimensión depende del numero de jugadores.

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>

pid_t hijo1, hijo2, hijo3, hijo4;
int n, numero = 0, numeroHijo1 = 0, numeroHijo2 = 0, numeroHijo3 = 0, numeroHijo4 = 0;

// Función Prueba CREAR HIJOS
void crearHijo(int numeroHijos)
{
    

    while(1);
}

int main()
{
    int i, j, DIMENSION, numero = 0, numeroHijo1 = 0, numeroHijo2 = 0, numeroHijo3 = 0, numeroHijo4 = 0;
    int valoresRepartidos, valoresIngresados;
    int cantidadJugadores = 2;
    pid_t hijo;
    bool continuarLlenado;

    srand(time(NULL));

    //* (P2) MATRIZ REAL *//
    if(cantidadJugadores == 2)
    {
        DIMENSION = 8;
        valoresRepartidos = 16;
    }
    
    if(cantidadJugadores == 3)
    {
        DIMENSION = 10;
        valoresRepartidos = 25;
    }

    if(cantidadJugadores == 4)
    {
        DIMENSION = 12;
        valoresRepartidos = 36;
    }

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

    //* Llenar matriz con valores del arreglo.
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
    

    // Imprimimos arreglo.
    printf("Arreglo:\n\n");

    for(i = 0; i < valoresRepartidos; i++)
    {
        printf("Posicion %d: [%d]\n", i, valoresMatriz[i]);
    }

    // Imprimimos matriz.
    printf("Matriz:\n\n");

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

    return 0;
}
