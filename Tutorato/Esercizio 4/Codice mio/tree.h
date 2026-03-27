#ifndef _TREE_H
#define _TREE_H

#include <stdbool.h>

/* --- DEFINIZIONE DELLA STRUTTURA --- 
   Questo blocco definisce come è fatto un singolo "pezzo" dell'albero binario.
*/

struct _tree_node {
  int key;                         // La chiave intera (usata per l'ordinamento)
  float val;                       // Il valore associato alla chiave
  struct _tree_node * left;        // Puntatore al figlio sinistro (chiavi minori)
  struct _tree_node * right;       // Puntatore al figlio destro (chiavi maggiori)
};

// Definisco 'tree' come un sinonimo di 'puntatore a un nodo', così il codice è più leggibile. 
typedef struct _tree_node * tree;

/* --- DICHIARAZIONE DELLE FUNZIONI (PROTOTIPI) --- 
   Queste sono le "promesse": diciamo al compilatore che queste funzioni 
   esistono e come devono essere chiamate.
*/

// Funzione per inserire un nuovo nodo o aggiornare uno esistente
tree insert(tree t, int key, float val);

// Funzione per cercare una chiave e restituire il valore tramite puntatore
bool search(tree t, int key, float * val);

// Funzione per visualizzare l'albero a video
void print_tree(tree t);

#endif

