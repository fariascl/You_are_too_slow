/*  Código destinado a realizar pruebas con el jugador.

    Historial de pruebas:

        1) Conectar proceso jugador con servidor mediante tubería [LISTO]
        2) Conectar proceso jugador con hijo del servidor [LISTO] -> Servidor reconoce pid del jugador.
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
    int fd, n, pid;
    char mensaje[20];

    // (P1) Abrir tubería.
    if((fd = open(FIFO, O_RDWR)) < 0)
    {
        perror("open");
        exit(1);
    }

    // (P2) Una vez creada la tubería enviamos pid al servidor para
    //  que se nos asigne un hijo.
    pid = getpid();

    write(fd, &pid, sizeof(pid));

    // (P1) Leer mensaje por teclado para enviar a la tubería.
    while((n = read(0, mensaje, 20)) > 0)
    {
        write(fd, &pid, sizeof(pid));
        write(fd, mensaje, n);
    }

    close(fd);

    return 0;
}