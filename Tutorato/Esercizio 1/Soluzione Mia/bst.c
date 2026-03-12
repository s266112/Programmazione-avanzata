// 4. FILE SORGENTE DELL'ALBERO: Scrivo i calcoli e la logica. 

#include "bst.h"
#include <stdio.h>
#include <errno.h>

// 1. Creo la struttura dell'albero
bst make_bst(void)
{
    bst tmp = (bst)malloc(sizeof(struct _bst));
    if (tmp == NULL)
    {
        errno = ENOMEM;  // Imposto errno a ENOMEM se malloc fallisce [cite: 17]
        perror ("make_bst: Creazione dell'albero fallita");
    }
    else
    {
        tmp->root = NULL; // All'inizio l'albero è vuoto, quindi radice e NULL
    }
    return tmp;         // Restituisce il puntatore all'albero creato
}

/* 2. Funzione statica (privata): Serve solo internamente per eliminare i nodi a cascata.
      Parte dalle foglie e risale fino alla radice*/

static void delete_node_cascade (t_node t)
{
    if (t == NULL) return;         // Se il nodo è NULL, non c'è nulla da liberare
    delete_node_cascade(t->left);  // Libero a sinistra
    delete_node_cascade(t->right); // Libero a destra
    delete_t_node(t);              // Uso la funzione definita in tree_node.c
}

// 3. Funzione pubblica per liberare tutto l'albero
void delete_bst(bst t)
{
    if (t != NULL)
    {
        delete_node_cascade(t->root); // Libero tutti i nodi partendo dalla radice
        free(t);                      // Libero la struttura dell'albero stessa
    }
}

/* La logica dell'inserimento:
   Se il nuovo valore è più grande del nodo corrente vai a DESTRA.
   Se è più piccolo, vai a SINISTRA*/

void bst_insert(bst t, t_node n)
{
    if (t == NULL || n == NULL) return; // Se l'albero o il nodo sono NULL, non posso inserire

    if (t->root == NULL)               // Se l'albero è vuoto, il nuovo nodo diventa la radice
    {
        t->root = n;                  // Imposto la radice dell'albero al nuovo nodo
    }
    else 
    { 
        t_node cfr = t->root; // Partiamo a confrontare dalla radice
        while (1)
        {
            if (n->key > cfr->key) // È più grande? Vai a destra
            {
                if (cfr->right == NULL) // Se non c'è un figlio destro, inserisco qui
                {
                    cfr->right = n;     // Imposto il figlio destro al nuovo nodo
                    n->parent = cfr;    // Imposto il genitore del nuovo nodo
                    break;              // Esco dal loop dopo aver inserito
                }
                else cfr = cfr->right; // Altrimenti, continuo a scendere a destra
            }
            else // È più piccolo o uguale? Vai a sinistra
            {
                if (cfr->left == NULL) // Se non c'è un figlio sinistro, inserisco qui
                {
                    cfr->left = n;      // Imposto il figlio sinistro al nuovo nodo
                    n->parent = cfr;    // Imposto il genitore del nuovo nodo
                    break;              // Esco dal loop dopo aver inserito
                }
                else cfr = cfr->left;  // Altrimenti, continuo a scendere a sinistra
            }
        }
    }
}

/* 4. Funzione "privata" per calcolare la profondità di un nodo
      Uso la logica: "la mia altezza è 1 + l'altezza del mio figlio più alto"*/

static int node_depth(t_node n)
{
    if (n == NULL) return 0;        // Se il nodo è NULL, la profondità è 0
    int l = node_depth(n->left);   // Calcolo la profondità del figlio sinistro
    int r = node_depth(n->right);  // Calcolo la profondità del figlio destro
    return (l > r ? l : r) + 1;    // Prende il massimo tra i due e aggiunge 1
}

// 5. Calcolo la profondità totale dell'albero
int bst_depth(bst t)
{
    if (t != NULL) return node_depth(t->root); // Se l'albero esiste, calcola
    return 0;
}