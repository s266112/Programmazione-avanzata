// 2. FILE SORGENTE: Metto la logica estratta dal file originale

#include "tree_node.h"
#include <stdio.h>
#include <errno.h>

// 1. Alloco un nuovo nodo e lo inizializzo a zero/NULL
t_node make_t_node(void)
{
    // Chiedo spazio per un nuovo nodo
    t_node tmp = (t_node)malloc(sizeof(struct _tree_node));

    // Se il PC non ha più memoria (raro, ma succede), tmp sarà NULL
    if (tmp == NULL)
    {
        errno = ENOMEM; // Setto l'errore di "memoria esaurita"
        perror ("make_t_node: Creazione del nodo fallita");
    }

    else

    // Se tutto va bene, pulisco il nodo mettendo tutto a NULL o zero
    {
        tmp->value = tmp->left = tmp->right = tmp->parent = NULL; // Inizializzo i puntatori a NULL
        tmp->key = 0;                                             // Inizializzo la chiave a 0
    }
    return tmp;     // Restituisco il nodo (o NULL se è fallito)

}


// 2. Funzione per distruggere un nodo e liberare memoria
void delete_t_node(t_node tmp)
{
    if (tmp == NULL)          // Se il nodo non esiste, non fare nulla
    return; 
    
    // Se dentro il nodo c'era un valore salvato, libero anche quello
    if (tmp->value != NULL) 
    {
        free(tmp->value); 
    }

    // Infine, libero la memoria del nodo stesso
    free(tmp); 
}                             

        