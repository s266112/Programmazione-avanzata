// Alessio Matessi  S266112

#include "tensorforth.h"
#include <stdlib.h>
#include <stdio.h>

// ================================================= //
// PARTE 1: TENSORI
// =================================================//

/*  --- CREA_TENSORE ---
    -> Alloca dinamicamente un nuovo tensore e la relativa area di memoria per i dati;
    -> Inizializza il contatore dei riferimenti a 1
*/
Tensore* crea_tensore (int32_t num_dim, int32_t* forma)
{
    // 1. Allocazione della struttura principale del Tensore
    Tensore* nuovo_tensore = (Tensore*)malloc(sizeof(Tensore));

    // Controllo di sicurezza: Se la memoria è piena, segnalo l'errore
    if (nuovo_tensore == NULL)
    {
        perror ("Errore: Impossibile allocare memoria per la struttura del Tensore");
        exit (EXIT_FAILURE);
    }

    // 2. Calcolo di quanti elementi totali conterrà il tensore e copia delle dimensioni
    size_t totale_elementi = 1;
    nuovo_tensore -> num_dim = num_dim;

    for (int i = 0; i < num_dim; i++)
    {
        nuovo_tensore -> forma[i] = forma[i];
        totale_elementi *= forma[i];
    }
    
    // 3. Allocazione dello spazio per i numeri (Uso calloc cosi inizializzo tutto a zero)
    nuovo_tensore -> dati = (float*)calloc(totale_elementi, sizeof (float));

   // Controllo di sicurezza per la seconda allocazione
   if (nuovo_tensore -> dati == NULL)
   {
    perror ("Errore: impossibile allocare memoria per i dati del Tensore");
    free (nuovo_tensore);         // Libero la struttura creata prima per evitare memory leak
    exit (EXIT_FAILURE);         // Esco con codice di errore
   }

   // 4. Inializzazione dei metadati di gestione memoria
   nuovo_tensore -> contatore_rif = 1;      // Il tensore è appena creato, c'è un solo riferimento attivo
   nuovo_tensore -> mmap_attivo = 0;        // E' stato allocato in RAM con calloc, non ancora letto dal disco

   return nuovo_tensore;
}

/*  --- TRATTIENI_TENSORE ---
    -> Incremento il contatore dei riferimenti del tensore
    -> Utilizzato per evitare copie fisiche dei dati durante operazioni logiche
*/
void trattieni_tensore(Tensore* t)
{
    // Controllo di sicurezza: verifico che il tensore esista davvero
    if (t != NULL)
    {
        t -> contatore_rif++;   // Incremento il contatore dei riferimenti
    }
}

/* --- RILASCIA_TENSORE --- 
    -> Decrementa il contatore dei riferimenti del tensore;
    -> Se il contatore raggiunge lo zero, dealloca fisicamente la memoria associata
*/
void rilascia_tensore (Tensore* t)
{
    // Controllo di sicurezza: Se il tensore è NULL, ignoro l'operazione
    if (t == NULL)
    {
        return;
    }

    t -> contatore_rif--;   // Decremento il contatore dei riferimenti

    // Se il contatore arriva a zero, nessun elemento del programma punta più a questo tensore. È sicuro distruggerlo.
    if (t -> contatore_rif == 0)
    {
        // 1. Devo prima liberare i dati veri e propri (l'array di float)
        // Se i dati sono stati creati in RAM (mmap_attivo == 0), uso free.
        if (t -> mmap_attivo == 0)
        {
            if (t -> dati != NULL)
            {
                free (t -> dati);
            }
        }
        else
        {
            // Nota per il futuro: qui gestiro la distribuzione tramite munmap(), quando leggero i file binari dal disco.
        }

        // 2. Infine, libero la struttura base (il Tensore stesso)
        free (t);
    }
}

// =============================================================== //
// PARTE 2: STACK
// ============================================================== //


/*  --- CREA_STACK ---
    -> Alloca e inizializza uno stack vuoto
*/
Stack* crea_stack(void)
{
    // 1. Chiedo al sistema operativo spazio per la struttura principale dello Stack
    Stack* nuovo_stack = (Stack*) malloc(sizeof(Stack));

    // 2. Controllo di sicurezza vitale
    if (nuovo_stack == NULL)
    {
        perror ("Errore: Impossibile allocare memoria per lo Stack");
        exit (EXIT_FAILURE);
    }

    // 3. All'inizio lo stack è completamente vuoto, il puntatore alla cima è NULL
    nuovo_stack -> cima = NULL;

    return nuovo_stack;
}


/*  --- PUSH ---
    -> Inserisce un tensore in cima allo stack
    -> Funziona come l'inserimento in testa di una sequenza
*/
void push(Stack*s, Tensore* t)
{
    // Controllo di sicurezza: se lo stack non esiste o il tensore è nullo, ignoro
    if (s == NULL || t == NULL)
    {
        return;
    }

    // 1. Creo il nuovo nodo della catena
    NodoStack* nuovo_nodo = (NodoStack*)malloc(sizeof(NodoStack));

    if (nuovo_nodo == NULL)
    {
        perror ("Errore: Impossibile allocare memoria per un NodoStack");
        exit (EXIT_FAILURE);
    }

    // 2. Inserisco il dato e aggancio il nuovo nodo alla vecchia cima
    nuovo_nodo -> tensore = t;                  // Inserisco il puntatore al tensore nel nodo
    nuovo_nodo -> prossimo = s -> cima;         // Il nuovo nodo punta a quello che prima era in cima

    // 3. Aggiorno la cima ufficiale dello stack
    s -> cima = nuovo_nodo;
}

/*  --- POP ---
    -> Rimuove e restituisce il tensore in cima allo stack
    -> Funziona come la rimozione della testa in una sequenza
*/
Tensore* pop (Stack* s)
{
    // 1. Controllo di sicurezza: se lo stack è vuoto, segnalo l'errore ed esco
    if (s == NULL || s -> cima == NULL)
    {
        fprintf (stderr, "Errore: tentativo di estrarre un elemento da uno stack vuoto!\n");
        exit (EXIT_FAILURE);   
    }

    // 2. Individuo il nodo da rimuovere (l'attuale cima)
    NodoStack* nodo_da_rimuovere = s -> cima;

    // 3. Salvo il puntatore al tensore prima di distruggere il nodo
    Tensore* tensore_estratto = nodo_da_rimuovere -> tensore;

    // 4. Aggiorno la cima: la nuova cima diventa il nodo sottostante
    s -> cima = nodo_da_rimuovere -> prossimo;

    // 5. Ora distruggo fisicamente il vecchio nodo
    free (nodo_da_rimuovere);

    // 6. Restituisco il tensore estratto
    return tensore_estratto;
}

/*  --- LIBERA_STACK ---
    -> Svuota completamente lo stack, rilasciando la memoria in modo sicuro
    -> Previene i memory leak alla chiusura dell'interprete
*/
void libera_stack(Stack* s)
{
    // Controllo di sicurezza: Se lo stack non esiste, non c'è nulla da liberare
    if (s == NULL)
    {
        return;
    }

    // Finchè la cima non è vuota, continuo a estrarre e distruggere
    while (s -> cima != NULL)
    {
        Tensore* t = pop(s);            // Estraggo il tensore attualmente in cima
        rilascia_tensore(t);            // Decremento i riferimenti (e dealloco se necessario)
    }

    // Ora che lo stack è completamente vuoto, posso distruggere la struttura base
    free(s);
}


