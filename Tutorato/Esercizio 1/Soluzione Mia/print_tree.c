// 6. FILE SORGEWNTE DELLA STAMPA (OPZIONALE!): Qui scrivo la logica per stampare l'albero in modo leggibile

#include "print_tree.h"
#include <stdio.h>

/* Funzione che stampa l'albero seguendo le regole:
   1. Se nullo -> "."
   2. Se foglia -> "chiave"
   3. Altrimenti -> "(chiave sinistra destra)" 
*/
void print_tree(t_node z) 
{
    // Regola 1: Se il nodo è NULL, stampa un punto [cite: 13]
    if (z == NULL) 
    {
        printf(".");
    }
    // Regola 2: Se è una foglia (non ha figli), stampa solo la chiave [cite: 13]
    else if (z->left == NULL && z->right == NULL) 
    {
        printf("%d", z->key);
    }
    // Regola 3: Se ha almeno un figlio, usa il formato (chiave sx dx) [cite: 14]
    else 
    {
        printf("(%d ", z->key);   // Stampa la chiave del nodo corrente
        print_tree(z->left);      // Chiama se stessa per il figlio sinistro
        printf(" ");              // Spazio di separazione
        print_tree(z->right);     // Chiama se stessa per il figlio destro
        printf(")");              // Chiude la parentesi
    }
}