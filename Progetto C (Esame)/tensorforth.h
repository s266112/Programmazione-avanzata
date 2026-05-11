// Alessio Matessi  S266112

#ifndef TENSORFORTH_H
#define TENSORFORTH_H

#include <stdint.h>
#include <sys/types.h>

/*Limite massimo di dimensioni supportate (vettori 1D o matrici 2D) */
#define MAX_DIM 2


// PARTE 1: TENSORI

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

/*  --- CREA_TENSORE --- */
Tensore* crea_tensore (int32_t num_dim, int32_t* forma);

/* --- TRATTIENI_TENSORE --- */
void trattieni_tensore (Tensore* t);

/* --- RILASCIA_TENSORE --- */
void rilascia_tensore(Tensore* t);

/*  --- STAMPA_TENSORE --- */
void stampa_tensore(Tensore* t);


// PARTE 2: STACK

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

/*  --- CREA_STACK --- */
Stack* crea_stack(void);

/*  --- PUSH --- */
void push (Stack* s, Tensore* t);

/*  --- POP --- */
Tensore* pop (Stack* s);

/*  --- LIBERA_STACK --- */
void libera_stack (Stack* s);


// PARTE 3: MATEMATICA E OPERAZIONI

/* 3.1 SOMMA TENSORI (+) */
void op_somma(Stack* s);

/* 3.2 SOTTRAZIONE TENSORI (-) */
void op_sottrazione(Stack* s);

/* 3.3 PRODOTTO TENSORI (*) */
void op_prodotto(Stack* s);


// PARTE 4: COMPARAZIONI, LOGICA E SELEZIONE

/* 4.1 MINORE (<) */
void op_minore(Stack* s);

/* 4.2 MAGGIORE (>) */
void op_maggiore(Stack* s);

/* 4.3 UGUALE (=) */
void op_uguale(Stack* s);

/* 4.4 AND LOGICO (&) */
void op_and(Stack* s);

/* 4.5 OR LOGICO (|) */
void op_or(Stack* s);

/* 4.6 NOT LOGICO (!) */
void op_not(Stack* s);

/* 4.7 SELEZIONE ($) */
void op_selezione(Stack* s);


// PARTE 5: MATEMATICA AVANZATA E STATISTICA

/* 5.1 PRODOTTO DI MATRICI (@) */
void op_prodotto_matrici(Stack* s);


#endif







