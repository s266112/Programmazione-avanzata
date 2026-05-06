# Alessio Matessi  S266112

# Definisco il compilatore e le bandierine
CC = gcc
CFLAGS = -Wall -Wextra -fopenmp

# Nome dell'eseguibile
TARGET = tensorforth

#Regola principale: quando scrivo 'make' compila tutto
all: $(TARGET)

# Costruisco l'eseguibile
$(TARGET): main.c tensorforth.c tensorforth.h
		$(CC) $(CFLAGS) main.c tensorforth.c -o $(TARGET)

# Regola: Pulisco la cartella
clean:
		rm -f $(TARGET)