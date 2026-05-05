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

/* 1. STRUTTURA DEL TENSORE 
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




// 2.FUNZIONI DI GESTIONE MEMORIA DEL TENSORE

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


// =============================================================== //
// PARTE 2: STACK
// ============================================================== //

/* 3. STRUTTURA DELLO STACK
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


// 4. FUNZIONI DI GESTIONE DELLO STACK

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
#endif







