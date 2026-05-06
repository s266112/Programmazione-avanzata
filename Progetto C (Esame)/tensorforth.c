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


/*  --- STAMPA_TENSORE ---
    -> Stampa a schermo il contenuto del tensore.
    -> Formatta l'output come vettore (1D) o matrice (2D) in base alle dimensioni.
*/
void stampa_tensore(Tensore* t)
{
    // 1. Controllo di sicurezza: Se il tensore o i dati non esistono, segnalo l'anomalia
    if (t == NULL)
    {
        printf ("Errore impossibile stampare, il tensore è NULL.\n");
        return;
    }
    if (t -> dati == NULL)
    {
        printf ("Errore: Il tensore non contiene dati allocati.\n");
        return;
    }

    // 2. Dimostrazione logica in base al numero di dimensioni
    if (t -> num_dim == 1)
    {
        // CASO 1D: Vettore: La dimensione [0] contiene il numero di elementi
        printf("[ ");
        for (int i = 0; i < t -> forma[0]; i++)
        {
            printf("%f ", t -> dati[i]);      // %f stampa i numeri float
        }
        printf("]\n");
    }
    else if (t -> num_dim == 2)
    {
        // CASO 2D: Matrice: La dimensione [0] sono le righe, mentre [1] sono colonne
        int righe = t -> forma[0];
        int colonne = t -> forma[1];

        // Doppio ciclo per simulare la griglia spaziale
        for (int r = 0; r < righe; r++)
        {
            printf("[ ");
            for (int c = 0; c < colonne; c++)
            {
                // Calcolo l'indice dell'array monodimensionale
                int indice = (r * colonne) + c;
                printf ("%f ", t -> dati[indice]);
            }
            printf("]\n");
        }
    }
    else
    {
        // Misura di sicurezza per evitare dimensioni non supportate dal progetto
        printf("Errore: Numero di dimensioni non supportato (%d).\n", t -> num_dim);
    }

    printf ("\n");           // Aggiungo uno spazio vuoto alla fine per ordine visivo
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


// ========================================================================= //
// PARTE 3: MATEMATICA E OPERAZIONI
// ======================================================================== //

/*  --- SOMMA TENSORI (+) ---
    -> Applica l'operatore + su due tensori.
    -> Parallelizzato tramite OpenMP.
*/
void op_somma(Stack*s)
{
    // 1. Estrazione operandi. Notazione (b a -- a+b )
    Tensore* b = pop(s);
    Tensore* a = pop(s);

    // 2. Controllo compatibilità dimensioni e forma
    if (a -> num_dim != b -> num_dim)
    {
        fprintf(stderr, "Errore: Numero di dimensioni incompatibile per la somma (+).\n");
        exit(EXIT_FAILURE);
    }

    int totale_elementi = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        if (a -> forma[i] != b -> forma[i])
        {
            fprintf(stderr, "Errore: Forma dei tensori incompatibile per la somma (+).\n");
            exit (EXIT_FAILURE);
        }
        totale_elementi *= a -> forma[i];       // Calcolo contestualmente quanti elementi ci sono
    }

    // 3. Allocazione del risultato
    Tensore* c = crea_tensore (a -> num_dim, a -> forma);

    // 4. Calcolo paralello
   
    #pragma omp parallel for                    // Dice al compilatore di dividere il ciclo for su più thread
    for (int i = 0; i < totale_elementi; i++)
    {
        c -> dati[i] = a -> dati[i] + b -> dati[i];
    }

    // 5. Aggiornamento stack e memoria
    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}


/*  --- SOTTRAZIONE TENSORI (-) ---
    -> Applica l'operatore - su due tensori.
    -> Parallelizzato tramite OpenMP.
*/
void op_sottrazione(Stack* s)
{
    // 1. Estrazione operandi. Notazione: (b a -- a-b)
    Tensore* a = pop(s);
    Tensore* b = pop(s);

    // 2. Controllo compatibilità dimensioni e forma
    if (a -> num_dim != b -> num_dim)
    {
        fprintf(stderr, "Errore: Numero di dimensioni incompatibili per la sottrazione (-).\n");
        exit(EXIT_FAILURE);
    }

    int totale_elementi = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        if (a -> forma[i] != b -> forma[i])
        {
            fprintf(stderr, " Errore: Forma dei tensori incompatibile per la sottrazione (-).\n");
            exit(EXIT_FAILURE);
        }
        totale_elementi *= a -> forma[i];
    }

    // 3. Allocazione del risultato
    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    // 4. Calcolo parallelo
    #pragma omp parallel for
    for (int i =0; i < totale_elementi; i++)
    {
        c -> dati[i] = a -> dati[i] - b -> dati[i];
    }

    // 5. Aggiornamento stack e memoria
    push (s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}


/*  --- PRODOTTO TENSORI (*) ---
    -> Applica l'operatore * su due tensori.
    -> Parallelizzato tramite OpenMP.
*/
void op_prodotto(Stack* s)
{
    // 1. Estrazione operandi Notazione: (b a -- a*b)
    Tensore* a = pop(s);
    Tensore* b = pop(s);

    // 2. Controllo compatibilità dimensioni e forma
    if (a -> num_dim != b -> num_dim)
    {
        fprintf(stderr, "Errore: Numero di dimensioni incompatibile per il prodotto (*).\n");
        exit(EXIT_FAILURE);
    }
    
    int totale_elementi = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        if (a -> forma[i] != b -> forma[i])
        {
            fprintf(stderr, "Errore: Forma dei tensori incompatibile per il prodotto (*).\n");
            exit(EXIT_FAILURE);
        }
        totale_elementi *= a -> forma[i];
    }

    // 3. Allocazione del risultato
    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    // 4. Calcolo parallelo
    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        c -> dati[i] = a -> dati[i] * b -> dati[i];
    }

    // 5. Aggiornamento stack e memoria
    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}


// ===================================================== //
// PARTE 4: COMPARAZIONI, LOGICA E SELEZIONE
// ===================================================== //


//     -------- A) COMPARAZIONI --------

/* MINORE (<) 
    -> Estrae due tensori  (a dalla cima, b da sotto).
    -> Esegue il confronto elemento per elemento (a < b).
    -> Inserisce un nuovo tensore risultato contenente 1.0 (Vero) o 0.0 (Falso).
*/
void op_minore(Stack* s)
{
    // 1. Estrazione operandi Notazione: (b a -- a<b)
    Tensore* a = pop(s);
    Tensore* b = pop(s);

    // 2. Controllo compatibilità dimensioni e forma
    if (a -> num_dim != b -> num_dim)
    {
        fprintf(stderr, "Errore: Numero di dimensioni incompatibile per l'operatore minore (<).\n");
        exit(EXIT_FAILURE);
    }
    
    int totale_elementi = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        if (a -> forma[i] != b -> forma[i])
        {
            fprintf(stderr, "Errore: Forma dei tensori incompatibile l'operatore minore (<).\n");
            exit(EXIT_FAILURE);
        }
        totale_elementi *= a -> forma[i];
    }

    // 3. Allocazione del risultato
    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    // 4. Calcolo parallelo
    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        // Se (a < b) e' vero assegna 1.0f, altrimenti 0.0f
        c -> dati[i] = (a -> dati[i] < b -> dati[i]) ? 1.0f : 0.0f;
    }

    // 5. Aggiornamento stack e memoria
    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}

/*  MAGGIORE (>) 
    -> Estrae due tensori (a dalla cima, b da sotto).
    -> Confronta elemento per elemento (a > b). 
    -> Inserisce 1.0 se vero, 0.0 se falso.
*/
void op_maggiore(Stack* s)

{
    // 1. Estrazione operandi Notazione: (b a -- a>b)
    Tensore* a = pop(s);
    Tensore* b = pop(s);

    // 2. Controllo compatibilità dimensioni e forma
    if (a -> num_dim != b -> num_dim)
    {
        fprintf(stderr, "Errore: Numero di dimensioni incompatibile per l'operatore maggiore (>).\n");
        exit(EXIT_FAILURE);
    }
    
    int totale_elementi = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        if (a -> forma[i] != b -> forma[i])
        {
            fprintf(stderr, "Errore: Forma dei tensori incompatibile l'operatore maggiore (>).\n");
            exit(EXIT_FAILURE);
        }
        totale_elementi *= a -> forma[i];
    }

    // 3. Allocazione del risultato
    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    // 4. Calcolo parallelo
    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        // Se (a > b) e' vero assegna 1.0f, altrimenti 0.0f
        c -> dati[i] = (a -> dati[i] > b -> dati[i]) ? 1.0f : 0.0f;
    }

    // 5. Aggiornamento stack e memoria
    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);

}

/*  UGUALE (=) 
    -> Estrae due tensori (a dalla cima, b da sotto).
    -> Confronta elemento per elemento (a == b). 
    -> Inserisce 1.0 se vero, 0.0 se falso.
*/
void op_uguale(Stack* s)

{
    // 1. Estrazione operandi Notazione: (b a -- a=b)
    Tensore* a = pop(s);
    Tensore* b = pop(s);

    // 2. Controllo compatibilità dimensioni e forma
    if (a -> num_dim != b -> num_dim)
    {
        fprintf(stderr, "Errore: Numero di dimensioni incompatibile per l'operatore uguale (=).\n");
        exit(EXIT_FAILURE);
    }
    
    int totale_elementi = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        if (a -> forma[i] != b -> forma[i])
        {
            fprintf(stderr, "Errore: Forma dei tensori incompatibile l'operatore uguale (=).\n");
            exit(EXIT_FAILURE);
        }
        totale_elementi *= a -> forma[i];
    }

    // 3. Allocazione del risultato
    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    // 4. Calcolo parallelo
    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        // Se (a == b) e' vero assegna 1.0f, altrimenti 0.0f
        c -> dati[i] = (a -> dati[i] == b -> dati[i]) ? 1.0f : 0.0f;
    }

    // 5. Aggiornamento stack e memoria
    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}



//     -------- B) LOGICA --------

/*  AND LOGICO (&) 
    -> Applica l'operatore logico AND elemento per elemento.
    -> I tensori in input devono contenere unicamente 0.0 e 1.0.
    -> Inserisce un nuovo tensore risultato contenente 1.0 o 0.0.
*/
void op_and(Stack* s)
{
    // 1. Estrazione operandi Notazione: (b a -- a&b)
    Tensore* a = pop(s);
    Tensore* b = pop(s);

    // 2. Controllo compatibilità dimensioni e forma
    if (a -> num_dim != b -> num_dim)
    {
        fprintf(stderr, "Errore: Numero di dimensioni incompatibile per l'operatore AND (&).\n");
        exit(EXIT_FAILURE);
    }
    
    int totale_elementi = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        if (a -> forma[i] != b -> forma[i])
        {
            fprintf(stderr, "Errore: Forma dei tensori incompatibile l'operatore AND (&).\n");
            exit(EXIT_FAILURE);
        }
        totale_elementi *= a -> forma[i];
    }

    // 3. Allocazione del risultato
    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    // 4. Calcolo parallelo
    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        // Se sia 'a' che 'b' sono veri assegna 1.0f, altrimenti 0.0f
        c -> dati[i] = (a -> dati[i] != 0.0f && b -> dati[i] != 0.0f) ? 1.0f : 0.0f;
    }

    // 5. Aggiornamento stack e memoria
    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}


/*  OR LOGICO (|) 
    -> Applica l'operatore logico OR elemento per elemento.
    -> Inserisce 1.0 se almeno uno dei due elementi e' vero (!= 0.0).
*/
void op_or(Stack* s)
{
    // 1. Estrazione operandi. Notazione: (b a -- a\|b)
    Tensore* a = pop(s);
    Tensore* b = pop(s);

    // 2. Controllo compatibilità dimensioni e forma
    if (a -> num_dim != b -> num_dim)
    {
        fprintf(stderr, "Errore: Numero di dimensioni incompatibili per l'operatore OR (|).\n");
        exit(EXIT_FAILURE);
    }
    
    int totale_elementi = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        if (a -> forma[i] != b -> forma[i])
        {
            fprintf(stderr, "Errore: Forma dei tensori incompatibili per l'operatore OR (|).\n");
            exit(EXIT_FAILURE);
        }
        totale_elementi *= a -> forma[i];
    }

    // 3. Allocazione del risultato
    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    // 4. Calcolo parallelo
    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        // OR logico: vero se 'a' o 'b' sono diversi da 0.0
        c -> dati[i] = (a -> dati[i] != 0.0f || b -> dati[i] != 0.0f) ? 1.0f : 0.0f;
    }

    // 5. Aggiornamento stack e memoria
    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}

/*  NOT LOGICO (!) 
    -> Operazione unaria: Estrae un singolo tensore.
    -> Inverte i valori logici (0.0 -> 1.0 e 1.0 -> 0.0).
*/
void op_not(Stack* s)
{
    // 1. Estrazione di UN SOLO operando. Notazione: a -- !a
    Tensore* a = pop(s);
    
    int totale_elementi = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        totale_elementi *= a -> forma[i];
    }

    // 2. Allocazione del risultato
    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    // 4. Calcolo parallelo
    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        // Se 'a' è 0.0, diventa 1.0. Altrimenti diventa 0.0.
        c -> dati[i] = (a -> dati[i] == 0.0f) ? 1.0f : 0.0f;
    }

    // 5. Aggiornamento stack e memoria
    push(s, c);
    rilascia_tensore(a);
}


//     -------- C) SELEZIONE --------

/*   OPERATORE DI SELEZIONE ($) 
    -> Estrae tre tensori e agisce come un operatore ternario o "multiplexer" (b a m -- m?a:b).
    -> 'm' è l'elemento in cima (maschera), 'a' è il secondo, 'b' è il terzo.
    -> Richiede che tutti e tre i tensori abbiano dimensioni e forme identiche.
*/
void op_selezione(Stack* s)
{
    // 1. Estrazione di tre operandi. Notazione: b a m -- m?a:b
    Tensore* m = pop(s);
    Tensore* a = pop(s);
    Tensore* b = pop(s);
    
    // 2. Controllo incrociato sul numero di dimensioni 
    if (a -> num_dim != b -> num_dim || a -> num_dim != m -> num_dim)
    {
        fprintf(stderr, "Errore: Numero di dimensioni incompatibili per l'operatore di selezione ($).\n");
        exit(EXIT_FAILURE);
    }
    
    // Controllo incrociato sulla forma
    int totale_elementi = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        if (a -> forma[i] != b -> forma[i] || a -> forma[i] != m -> forma[i])
        {
            fprintf(stderr, "Errore: Forma dei tensori incompatibili per l'operatore di selezione ($).\n");
            exit(EXIT_FAILURE);
        }
        totale_elementi *= a -> forma[i];
    }

    // 3. Allocazione del risultato
    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    // 4. Calcolo parallelo
    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        // Se m è diverso da 0.0 (Vero), seleziona l'elemento da 'a', altrimenti da 'b'
        c -> dati[i] = (m -> dati[i] != 0.0f) ? a -> dati[i] : b -> dati[i];
    }

    // 5. Aggiornamento stack e memoria
    push(s, c);
    rilascia_tensore(m);
    rilascia_tensore(a);
    rilascia_tensore(b);
}












