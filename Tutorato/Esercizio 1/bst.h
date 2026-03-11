/* 3. FILE HEADER DELL'ALBERO: Definisco la struttura dell'albero e le 
                            operazioni che si possono fare con esso */


# ifndef BST_H
# define BST_H

#include "tree_node.h" // Serve per usare la struttura t_node

// 1. Definizione della struttura che tiene traccia della radice
struct _bst
{
    t_node root; // Il nodo di partenza, in cima a tutto
};

// 2. Alias per rendere il codice più leggibile
typedef struct _bst *bst;   // Scrivo 'bst' invece di 'struct _bst *

// 3. Prototipi delle funzioni per gestire l'albero

bst make_bst(void);                 // Crea un nuovo albero vuoto
void delete_bst(bst t);            // Distrugge l'albero e tutti i suoi nodi
void bst_insert(bst t, t_node n); // Inserisce un nodo al posto giusto
int bst_depth(bst t);            // Calcola la profondità dell'albero

# endif