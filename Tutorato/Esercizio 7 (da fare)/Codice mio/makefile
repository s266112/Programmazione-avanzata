# --- CONFIGURAZIONE AMBIENTE ---
# CC: Definiamo il compilatore (GNU C Compiler)
# CFLAGS: Flag di ottimizzazione e standard minimi richiesti
CC = gcc
CFLAGS = -O3 -Wall -std=c17 -pedantic -fopenmp -D_POSIX_C_SOURCE=200809L

# 1. TARGET PRINCIPALE
# La regola 'all' è quella che viene eseguita di default quando scrivi 'make'
all: main

# 2. LINKING FINALE
# Qui assembliamo i tre moduli (.o) per creare l'eseguibile 'main'.
# $^ indica tutti i prerequisiti, $@ indica il nome del target (main)
main: main.o netpbm.o image_transform.o
	$(CC) $(CFLAGS) $^ -o $@

# 3. GESTIONE DIPENDENZE HEADER
# Fondamentale: se modifichi un file .h, Make capisce che deve ricompilare 
# i moduli .c corrispondenti per aggiornare le costanti (es. RADIUS).
main.o: netpbm.h image_transform.h
netpbm.o: netpbm.h
image_transform.o: image_transform.h netpbm.h

# 4. REGOLA DI COMPILAZIONE GENERICA
# Trasforma ogni file .c nel rispettivo file oggetto .o
# $< indica il primo prerequisito (il file .c)
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# 5. COMANDI DI SERVIZIO
# .PHONY evita conflitti se esistessero file chiamati 'all' o 'clean'
.PHONY: all clean

# La regola 'clean' riporta il progetto allo stato iniziale (pulisce i binari e le immagini)
clean:
	rm -f *.o
	rm -f main
	rm -f kitty-transformed-*.pgm