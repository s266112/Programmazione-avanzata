# --- VARIABILI ---
# Definisco quale compilatore usare (gcc)
CC = gcc

# Opzioni di compilazione:
# -O3: Ottimizzazione massima (rende il codice più veloce per i benchmark)
# -Wall: Attiva tutti gli avvisi di errore (Warning all)
# -std=c17: Usa lo standard del linguaggio C del 2017
# -pedantic: Obbliga a seguire le regole del C in modo molto rigoroso
CFLAGS = -O3 -Wall -std=c17 -pedantic

# Cerca automaticamente tutti i file .h nella cartella corrente
HEADERS = $(wildcard *.h)

# Elenco dei file oggetto (i file compilati ma non ancora collegati tra loro)
OBJECTS = main.o tree.o ctree.o


# --- REGOLE ---

# La regola di default: se scrivi solo "make", lui cercherà di creare "main"
all: main

# Come creare l'eseguibile finale "main":
# Ha bisogno dei file oggetto e degli header. 
# $@ indica il nome del bersaglio (in questo caso 'main')
main: ${OBJECTS} ${HEADERS}
	${CC} ${CFLAGS} ${OBJECTS} -o $@

# Come creare i singoli file .o partendo dai file .c:
# % è un carattere jolly. $< indica il file sorgente (.c)
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $<

# .PHONY dice a make che "clean" non è un file fisico, ma solo un comando
.PHONY: clean

# Regola per pulire la cartella: rimuove tutti i file .o
# Utile per ricompilare tutto da zero senza vecchi residui
clean:
	rm -f *.o
