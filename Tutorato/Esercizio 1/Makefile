# --- SEZIONE 1. LE VARIABILI: --- 
# Scrivo subito le variabili, cosi se voglio cambiare il compilatore 
# o le opzioni di compilazione, mi basta modificare queste righe e non tutto il Makefile


# Variabili per il compilatore e le opzioni
CC = gcc
CFLAGS = -Wall -Wextra -g 	#-Wall e -Wextra: Attivano tutti i warning
LIB_NAME = libbst

# Variabile per decidere se la libreria è statica (0) o dinamica (1)
SHARED = 0

# Lista dei file oggetto necessari per la libreria
LIB_OBJS = tree_node.o bst.o print_tree.o



# --- SEZIONE 2: LOGICA DELLA LIBRERIA: ---
# Qua decido se creare una libreria .a (statica) o .so (dinamica)

ifeq ($(SHARED), 1)

	# BLOCCO DINAMICO: Se SHARED è 1, creo la libreria dinamica
	LIB_TARGET = $(LIB_NAME).so

	# Il flag -fPIC è obbligatorio per le librerie dinamiche
	CFLAGS += -fPIC

	# LDFLAGS dice al programma dove cercare la libreria
	LDFLAGS = -L. -lbst -Wl,-rpath,.

else

	# BLOCCO STATICO: Se SHARED è 0, creo la libreria statica
	LIB_TARGET = $(LIB_NAME).a

	# Per la libreria statica, basta includere il file .a direttamente
	LDFLAGS = $(LIB_TARGET)

endif


# --- SEZIONE 3. LE REGOLE: ---
# Qua dico come trasformare i file .c in .o, e come creare la libreria finale


# Il target "all" è quello che viene eseguito di default
all: main

# Per fare il 'main', servono l'oggetto main.o e la libreria
main: main.o $(LIB_TARGET)
	$(CC) $(CFLAGS) main.o -o main $(LDFLAGS)

# Regola per creare la libreria STATICA (.a)
$(LIB_NAME).a: $(LIB_OBJS)
	ar rcs $@ $^

# Regola per creare la libreria DINAMICA (.so)
$(LIB_NAME).so: $(LIB_OBJS)
	$(CC) -shared -o $@ $^

# Regola generale: trasforma ogni file .c in un file .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regola per pulire i file generati (utile per ricompilare da zero)
clean:
	rm -f *.o main $(LIB_NAME).a $(LIB_NAME).so

	





