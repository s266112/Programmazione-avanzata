#ifndef _CTREE_H
#define _CTREE_H

#include <stdbool.h>

/* --- COSTANTE DI GRADO --- 
   N definisce quante chiavi massime può contenere un singolo nodo.
   In questo caso N=3, quindi ogni nodo può avere fino a 3 chiavi e 4 figli.
*/
#define N 3

/* --- STRUTTURA DEL NODO N-ARIO --- */
struct _ctree_node 
{
  int key[N];                         // Array che ospita le chiavi (interi)
  float val[N];                       // Array che ospita i valori (decimali)
  
  int first_free;                     // Indice che indica quante chiavi sono presenti attualmente nel nodo (da 0 a N)
  
  struct _ctree_node * children[N+1]; // Array di puntatori ai figli. Se ho N chiavi, ho N+1 possibili rami.
};

/* Definisco 'ctree' come sinonimo di 'puntatore a un nodo ctree_node' per rendere il codice più pulito. */
typedef struct _ctree_node * ctree;


/* --- PROTOTIPI DELLE FUNZIONI --- 
   Queste sono le dichiarazioni delle funzioni che hai implementato in ctree.c
*/

// Funzione per inserire dati nell'albero n-ario
ctree cinsert(ctree t, int key, float val);

// Funzione per cercare una chiave nell'albero n-ario
bool csearch(ctree t, int key, float * val);

// Funzione per stampare la struttura dell'albero a video
void print_ctree(ctree t);

#endif

