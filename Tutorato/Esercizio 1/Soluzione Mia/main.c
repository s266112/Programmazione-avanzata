// 7. IL FILE PRINCIPALE: Qui testo tutta la logica scritta nei file precedenti

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bst.h"         // Includiamo l'albero per poter usare le sue funzioni
#include "print_tree.h"  // Per usare la funzione di stampa

/* 1. Test Sequenziale: aggiunge nodi con chiavi in ordine (0, 1, 2...) 
   Questo crea un albero "sbilanciato" (una lunga linea dritta) */
void s_test(int n) 
{
    int i = 0;
    bst t = make_bst(); // Creiamo un nuovo albero vuoto
    printf("\n--- Test Sequenziale ---\n");
    printf("Aggiunta di %d nodi...\n", n);
    
    for (; i < n; i++) 
    {
        t_node node = make_t_node(); // Creiamo un nuovo nodo
        node->key = i;               // Assegniamo la chiave (0, 1, 2...)
        bst_insert(t, node);         // Lo inseriamo nell'albero
    }

    // Stampo l'albero risultante 
    printf("Struttura dell'albero: ");
    print_tree(t->root); 
    printf("\n");
    
    printf("L'albero sequenziale e' profondo %d\n", bst_depth(t));
    delete_bst(t); // Liberiamo tutta la memoria usata 
}

/* 2. Test Casuale: aggiunge nodi con chiavi generate casualmente 
   Questo crea un albero più distribuito e "basso" */
void r_test(int n) 
{
    int i = 0;
    bst t = make_bst();
    printf("\n--- Test Casuale ---\n");
    printf("Aggiunta di %d nodi...\n", n);
    
    for (; i < n; i++) 
    {
        t_node node = make_t_node();
        node->key = rand() % 100;   // Uso numeri piccoli per leggerlo meglio
        bst_insert(t, node);
    }

    // Stampo l'albero risultante
    printf("Struttura dell'albero: ");
    print_tree(t->root);
    printf("\n");
    
    printf("L'albero casuale e' profondo %d\n", bst_depth(t));
    delete_bst(t); // Liberiamo la memoria 
}

int main(int argc, char * argv[]) 
{
    int n;
    srand((int)time(NULL)); // Inizializza il generatore di numeri casuali
    
    printf("Inserire il numero di nodi: ");
    /* Legge il numero di nodi dall'utente. 
       Il ciclo while serve a riprovare se l'input non è un numero. */
    while (scanf("%d", &n) != 1) 
    {
        printf("Per favore, inserisci un numero: ");
        while (getchar() != '\n'); // Pulisce il buffer dell'input
    }
    
    s_test(n); // Esegue il test con numeri ordinati
    r_test(n); // Esegue il test con numeri casuali
    
    return 0;
}