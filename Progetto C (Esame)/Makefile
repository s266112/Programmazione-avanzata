# Alessio Matessi  S266112

# Definisco il compilatore 
CC = gcc
CFLAGS = -Wall -Wextra -fopenmp

# Nome dell'eseguibile
TARGET = tensorforth

# Quando scrivo 'make' compila tutto
all: $(TARGET)

# Costruisco l'eseguibile
$(TARGET): main.c tensorforth.c tensorforth.h
	$(CC) $(CFLAGS) main.c tensorforth.c -o $(TARGET)

# Pulisco la cartella
clean:
	@echo "Inizio pulizia file temporanei..."
	rm -f $(TARGET) *.o
	@# Rimuove solo i file generati, senza toccare gli originali
	rm -f examples/tensor.bin
	rm -f examples/*-blurred.pgm
	rm -f examples/*-bw.pgm
	rm -f examples/*-edges.pgm
	rm -f examples/gol_*.pgm
	@echo "Pulizia completata."

.PHONY: all clean