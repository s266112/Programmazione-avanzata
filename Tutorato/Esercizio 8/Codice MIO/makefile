# 1. VARIABILI DI CONFIGURAZIONE:	- CC: Definisco il compilatore da usare (gcc).
# 									- CFLAGS: Opzioni di ottimizzazione (-O3) e standard di sicurezza.
#         							- -fPIC è fondamentale per creare librerie condivise.
CC = gcc
CFLAGS = -O3 -fPIC -std=c17 -pedantic


# 2. TARGET PRINCIPALE (ALL): È l'obiettivo finale: voglio ottenere il file libtree.so.
all: libtree.so


# 3. CREAZIONE LIBRERIA CONDIVISA (.SO): Prendo il file oggetto (.o) e lo trasformo in una libreria
# 										 che può essere caricata esternamente (opzione -shared).
libtree.so: tree.o
	${CC} ${CFLAGS} -shared $< -o $@


# 4. COMPILAZIONE FILE OGGETTO (.O): Trasformo il codice sorgente (.c) in codice macchina intermedio.
# 									 Questo passaggio avviene solo se i file .c o .h sono cambiati.
%.o: %.c %.h
	${CC} ${CFLAGS} -c $<


# 5. COMANDI DI SERVIZIO (PHONY): 'clean' serve a fare pulizia eliminando i file temporanei.
.PHONY: clean all

clean:
	rm -rf *.o libtree.so