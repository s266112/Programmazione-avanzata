# Definisce il compilatore da usare
CC = gcc

# -03: Ottimizzazione massima (rende la mia unrolled più veloce)
# - Wall: Mostra tutti gli avvertimenti (errori di distrazione)
CFLAGS = -O3 -Wall -std=c17 -pedantic

# Cerca tutti i file .h nella cartella
HEADERS = ${wildcard *.h}

# Elenco dei file oggetto da creare
OBJECTS = main.o linked_list.o unrolled_linked_list.o

all: main

# Crea l'eseguibile "main" unendo i tre file oggetto
main: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ $^

# Compila i file .c in un file .o
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $<

.PHONY: clean

# comando per pulire la cartella dai file compilati (scrivo 'make clean')
clean:
	rm -f *.o
