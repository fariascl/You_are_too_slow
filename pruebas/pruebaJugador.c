/*  Código destinado a realizar pruebas con el jugador.

    Historial de pruebas:

        1) Conectar proceso jugador con servidor mediante tubería [LISTO]
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

// Nombre de la tubería.
#define FIFO "tuberia"

int main()
{
    // (P1) Variables
    int fd, n;
    char mensaje[20];

    // (P1) Abrir tubería.
    if((fd = open(FIFO, O_RDWR)) < 0)
    {
        perror("open");
        exit(1);
    }

    // (P1) Leer mensaje por teclado para enviar a la tubería.
    while((n = read(0, mensaje, 20)) > 0)
    {
        write(fd, mensaje, n);
    }

    close(fd);

    return 0;
}