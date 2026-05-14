#ifndef _TREE_H
#define _TREE_H

#include <stdbool.h>

/* --- 1. DEFINIZIONE STRUTTURA (NODE) ---
  -> Rappresenta lo scheletro del nodo in memoria. 
  -> È la mappa che Python legge per capire dove si trovano le chiavi e i valori.
*/
struct _tree_node 
{
  // CARICO UTILE: I dati salvati nel nodo (Chiave e Valore).
  int key;
  float val;
  
  // PUNTATORI AI FIGLI: Gli indirizzi di memoria per muoversi nell'albero. 
  struct _tree_node * left;
  struct _tree_node * right;
};


/*  --- 2. DEFINIZIONE TIPO (TREE) ---
 -> Creo un alias: d'ora in poi 'tree' significa "puntatore a un nodo".
 -> Mi serve per scrivere codice più pulito e leggibile.
*/
typedef struct _tree_node * tree;



/* --- 3. PROTOTIPI DELLE FUNZIONI ---
  -> Elenco le funzioni disponibili nella libreria. 
  -> Le loro implementazioni reali si trovano nel file 'tree.c'.
*/

// Inserisco un nuovo nodo o aggiorno quello esistente. 
tree insert(tree t, int key, float val);

// Cerco una chiave e salvo il risultato nell'indirizzo puntato da 'val'. 
bool search(tree t, int key, float * val);

// Distruggo l'intero albero liberando la memoria.
void destroy(tree t);

#endif
