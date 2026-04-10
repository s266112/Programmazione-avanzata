# --- CONFIGURAZIONE DEL COMPILATORE ---
CC = gcc

# OPZIONI DI COMPILAZIONE:
# -O3: Attiva le ottimizzazioni massime del compilatore
# -march=native: Dice al compilatore di ottimizzare il codice specificamente per la tua CPU
# -Wall -pedantic: Attiva tutti gli avvisi per scrivere un codice pulito e senza errori
CFLAGS = -O3 -march=native -std=c17 -Wall -pedantic

# Elenco dei file header del progetto
HEADERS = matrix.h matrix_multiply.h

# Elenco dei file oggetto (i componenti compilati)
OBJECTS = main.o matrix.o matrix_multiply.o

# --- REGOLE DI COMPILAZIONE ---

# Regola principale: digita 'make' per creare l'eseguibile 'main'
all: main

# Come assemblare l'eseguibile finale dai file oggetto
main: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o main

# Come compilare i singoli file .c in file .o
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Regola per pulire la cartella dai file temporanei
.PHONY: clean
clean:
	rm -f *.o main
