# --- CONFIGURAZIONE ---
CC = gcc
# -O3: Ottimizzazione massima (velocizza i calcoli della trasposizione)
# -Wall -pedantic: Attiva tutti gli avvisi per un codice pulito
# -std=c17: Usa lo standard C del 2017
CFLAGS = -O3 -Wall -std=c17 -pedantic

# Obiettivo predefinito: crea l'eseguibile 'main'
all: main

# --- ASSEMBLAGGIO FINALE ---
# Collega i file oggetto (.o) per creare il file binario finale
main: main.o netpbm.o netpbm.h
	${CC} ${CFLAGS} main.o netpbm.o -o $@

# --- COMPILAZIONE MODULI ---
# Trasforma ogni file .c in un file oggetto .o
# $^ indica tutte le dipendenze (i file .c e .h)
%.o: %.c netpbm.h
	${CC} ${CFLAGS} -c $^

# Comandi di servizio
.PHONY: clean main

# Pulisce la cartella dai file temporanei per ricominciare da zero
clean:
	rm -rf *.o main
