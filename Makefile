CC=gcc
CFLAGS=-pthread
you_are_too_slow: servidor cliente
  mv servidor bin/
  mv cliente bin/
servidor: servidor.c
  mkdir bin
  $(CC) $(CFLAGS) servidor.c -o servidor

cliente: cliente.c
  $(CC) $(CFLAGS) cliente.c -o cliente
