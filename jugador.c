/*
    ╔═════════════════════════════╗ 
        PROCESO CLIENTE JUGADOR    
    ╚═════════════════════════════╝

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

/* .: TUBERÍA :. */
#define FIFO "tuberia"

/* .: PROGRAMA PRINCIPAL :. */
int main()
{
    /* .: VARIABLES :. */
    int fd, n, pid;
    char mensaje[20];

    /* .: ABRIR TUBERÍA PARA COMUNICARSE CON EL SERVIDOR :. */
    if((fd = open(FIFO, O_RDWR)) < 0)
    {
        perror("open");
        exit(1);
    }

    // Enviar PID por la tubería para que se nos asigne un Cliente Servidor.
    pid = getpid();

    write(fd, &pid, sizeof(pid));
    write(fd, &pid, sizeof(pid));

    return 0;
}