// Alessio Matessi  S266112

#ifndef TENSORFORTH_H
#define TENSORFORTH_H

#include <stdint.h>
#include <sys/types.h>


/*Limite massimo di dimensioni supportate (vettori 1D o matrici 2D) */
#define MAX_DIM 2

// ================================================================ //
// PARTE 1: TENSORI
// =============================================================== //

/* 1.1 STRUTTURA DEL TENSORE 
-> Rappresenta l'unità di dato fondamentale manipolata dall'interprete.
-> Contiene i valori matematici, i dettagli sulla dimensionalità e i metadati necessari per la gestione efficiente della memoria.
*/

typedef struct 
{
    float* dati;                // Puntatore all'area di memoria contenente i valori effettivi
    int32_t forma[MAX_DIM];     // Array delle dimensioni (Es: numerio di righe/colonne)
    int32_t num_dim;            // Numero di dimensioni attive (1 per vettori, 2 per matrici)


    int32_t contatore_rif;      // Contatore dei riferimenti per ottimizzare le duplicazioni
    int mmap_attivo;            // Flag: 1 se il file è mappato in sola lettura (mmap), 0 altrimenti
} Tensore;




// 1.2 FUNZIONI DI GESTIONE MEMORIA DEL TENSORE

/*  --- CREA_TENSORE ---
    -> Alloca dinamicamente un nuovo tensore e la relativa area di memoria per i dati.
    -> Inizializza il contatore dei riferimenti a 1;
    -> num_dim: numero di dimensioni (1 o 2);
    -> forma: Array contenente l'estensione di ciascuna dimensione. 
*/
Tensore* crea_tensore (int32_t num_dim, int32_t* forma);


/* --- TRATTIENI_TENSORE ---
    -> Incrementa il contatore dei riferimenti del tensore;
    -> Utilizzato per evitare copie fisiche dei dati durante operazioni logiche 
*/
void trattieni_tensore (Tensore* t);


/* --- RILASCIA_TENSORE ---
    -> Decrementa il contatore dei riferimenti del tensore;
    -> Se il contatore raggiunge lo zero, dealloca fisicamente la memoria associata.
*/
void rilascia_tensore(Tensore* t);


/*  --- STAMPA_TENSORE ---
    -> Stampa a schermo il contenuto del tensore.
    -> Formatta l'output come vettore (1D) o matrice (2D) in base alle dimensioni.
*/
void stampa_tensore(Tensore* t);


// =============================================================== //
// PARTE 2: STACK
// ============================================================== //

/* 2.1 STRUTTURA DELLO STACK
    -> L'interprete si basa su una pila LIFO (Last In, First Out).
    -> Implemento lo stack come in una lista concatenata dove gli inserimenti e le rimozioni avvengono esclusivamente dalla "cima" 
*/

// Nodo della lista concatenata
typedef struct NodoStack
{
    Tensore* tensore;                   // Il dato è contenuto in questo nodo
    struct NodoStack* prossimo;         // Puntatore all'elemento sottostante nella pila
} NodoStack;

// Struttura principale dello stack
typedef struct
{
    NodoStack* cima;                    // Puntatore all'elemento più in alto dello stack
} Stack;


// 2.2 FUNZIONI DI GESTIONE DELLO STACK

/*  --- CREA_STACK ---
    -> Alloca e inizializza uno stack vuoto
*/
Stack* crea_stack(void);

/*  --- PUSH ---
    -> Inserisce un tensore in cima allo stack
*/
void push (Stack* s, Tensore* t);

/*  --- POP ---
    -> Rimuove e restituisce il tensore in cima allo stack
    -> Se lo stack è vuoto, deve gestire l'errore in modo sicuro
*/
Tensore* pop (Stack* s);

/*  --- LIBERA_STACK ---
    -> Svuota completamente lo stack, rilasciando tutti i tensori rimasti e liberando la memoria dei nodi.
*/
void libera_stack (Stack* s);


// ========================================================= //
// PARTE 3: MATEMATICA E OPERAZIONI
// ======================================================== //

/* 3.1 SOMMA TENSORI (+) 
    -> Estrae due tensori dallo stack (B dalla cima, poi A).
    -> Controlla che abbiano dimensioni compatibili.
    -> Somma elemento per elemento e inserisce il tensore risultato nello stack.
*/
void op_somma(Stack* s);

/* 3.2 SOTTRAZIONE TENSORI (-) 
    -> Estrae due tensori dallo stack.
    -> Sottrae elemento per elemento (cima - sotto) e inserisce il risultato.
*/
void op_sottrazione(Stack* s);

/* 3.3 PRODOTTO TENSORI (*) 
    -> Estrae due tensori dallo stack.
    -> Moltiplica elemento per elemento e inserisce il risultato.
*/
void op_prodotto(Stack* s);



// ===================================================== //
// PARTE 4: COMPARAZIONI E LOGICA
// ===================================================== //

/* 4.1 MINORE (<) 
    -> Estrae due tensori  (a dalla cima, b da sotto).
    -> Esegue il confronto elemento per elemento (a < b).
    -> Inserisce un nuovo tensore risultato contenente 1.0 (Vero) o 0.0 (Falso).
*/
void op_minore(Stack* s);

/* 4.2 MAGGIORE (>) 
    -> Estrae due tensori (a dalla cima, b da sotto).
    -> Confronta elemento per elemento (a > b). 
    -> Inserisce 1.0 se vero, 0.0 se falso.
*/
void op_maggiore(Stack* s);

/* 4.3 UGUALE (=) 
    -> Estrae due tensori (a dalla cima, b da sotto).
    -> Confronta elemento per elemento (a == b). 
    -> Inserisce 1.0 se vero, 0.0 se falso.
*/
void op_uguale(Stack* s);

/* 4.4 AND LOGICO (&) 
    -> Estrae due tensori (a dalla cima, b da sotto).
    -> I tensori devono contenere solo 0.0 o 1.0.
    -> Applica l'operazione logica AND elemento per elemento.
*/
void op_and(Stack* s);

/* 4.5 OR LOGICO (|) 
    -> Estrae due tensori (a dalla cima, b da sotto).
    -> I tensori devono contenere solo 0.0 o 1.0.
    -> Applica l'operazione logica OR elemento per elemento.
*/
void op_or(Stack* s);

/* 4.6 NOT LOGICO (!) 
    -> Estrae UN SOLO tensore dalla cima dello stack.
    -> Il tensore deve contenere solo 0.0 o 1.0.
    -> Applica la negazione logica (0.0 diventa 1.0, 1.0 diventa 0.0).
*/
void op_not(Stack* s);

/* 4.7 SELEZIONE ($)
    -> Estrae tre tensori dallo stack (m dalla cima, a dal centro, b da sotto).
    -> Il tensore 'm' agisce da maschera booleana (0.0 o 1.0).
    -> Se l'elemento di m e' vero (1.0), seleziona l'elemento di a, altrimenti di b.
*/
void op_selezione(Stack* s);



#endif







