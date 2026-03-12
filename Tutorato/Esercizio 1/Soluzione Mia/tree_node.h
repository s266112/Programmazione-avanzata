// 1. FILE HEADER: Definisco la strutture e i prototipi.
// Uso le "Header Guards (#ifndef....)" per evitare inclusione multiple


# ifndef TREE_NODE_H
# define TREE_NODE_H

#include <stdlib.h>

// 1. Definizione della struttura dell nodo

/* SPIEGO: Questa è la struttura base del "mattone"
   Contine la chiave, un puntatore a dati generici e i 
   collegamenti ai "vicini" (sinistro, destro e genitore)*/

struct _tree_node
{
    int key;                       // Valore numerico del nodo (la chiave)
    void *value;                  // Puntatore a qualsiasi tipo di dato (opzionale)
    struct _tree_node *left;     // Figlio sinistro
    struct _tree_node *right;   // Figlio destro
    struct _tree_node *parent; // Nodo genitore
};

// 2. Alias per rendere il codice più leggibile

/* SPIEGO: Invece di scrivere ogni volta 'struct _tree_node *'
           scrivo sempliicemente 't_node'*/

typedef struct _tree_node *t_node;

// 3. Funzione per gestire il ciclo di vita del nodo

/* SPIEGO: Qui metto solo le "promesse" (prototipi).
           Le funzioni esistono, ma la ricetta vera e propria e nel file .c*/

t_node make_t_node(void);           // Creo un nodo vuoto
void delete_t_node(t_node tmp);     // Distruggo un nodo e libero la memoria associata

# endif

