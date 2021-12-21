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

pid_t hijo1, hijo2, hijo3, hijo4;
int n, numero = 0, numeroHijo1 = 0, numeroHijo2 = 0, numeroHijo3 = 0, numeroHijo4 = 0;

// Función Prueba CREAR HIJOS
void crearHijo(int numeroHijos)
{
    

    while(1);
}

int main()
{
    int i, numero = 0, numeroHijo1 = 0, numeroHijo2 = 0, numeroHijo3 = 0, numeroHijo4 = 0;
    int jugadores = 4;
    pid_t hijo;

    for(i=1; i<=jugadores; i++)
    {
        hijo = fork();

        if(hijo == 0)
        {
            break;
        }
    }

    if(hijo == 0)
    {
        printf("Soy un hijo || PID %d\n", getpid());
    }

    printf("HOLAAAAAAAA\n");

    while(1);

    return 0;
}
