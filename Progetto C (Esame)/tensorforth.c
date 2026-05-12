// Alessio Matessi  S266112

#include "tensorforth.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* ----------   PARTE 1: TENSORI    ---------- */

/*  --- CREA_TENSORE ---
    -> Alloca dinamicamente un nuovo tensore e la relativa area di memoria per i dati;
    -> Inizializza il contatore dei riferimenti a 1
*/
Tensore* crea_tensore (int32_t num_dim, int32_t* forma)
{
    // 1. Allocazione della struttura base del Tensore
    Tensore* nuovo_tensore = (Tensore*)malloc(sizeof(Tensore));

    // 2. Controllo: Se la memoria è piena segnalo l'errore ed esco
    if (nuovo_tensore == NULL)
    {
        perror ("Errore: Impossibile allocare memoria per la struttura del Tensore");
        exit (EXIT_FAILURE);
    }

    // 3. Registrzione dimensioni e calcolo totale degli elementi
    size_t totale_elementi = 1;
    nuovo_tensore -> num_dim = num_dim;

    for (int i = 0; i < num_dim; i++)
    {
        nuovo_tensore -> forma[i] = forma[i];
        totale_elementi *= forma[i];
    }
    
    // 4. Allocazione array dei dati (inizializzato a zero)
    nuovo_tensore -> dati = (float*)calloc(totale_elementi, sizeof (float));

    if (nuovo_tensore -> dati == NULL)
    {
        perror ("Errore: Impossibile allocare memoria per i dati del Tensore");
        free (nuovo_tensore);         // Libero la struttura creata prima per evitare memory leak
        exit (EXIT_FAILURE);         // Esco con codice di errore
    }

   // 5. Inizializzazione dei metadati di gestione memoria
   nuovo_tensore -> contatore_rif = 1;      // Tensore appena creato: 1 solo riferimento attivo
   nuovo_tensore -> mmap_attivo = 0;        // Allocato in RAM (non ancora mappato)
   return nuovo_tensore;
}


/*  --- TRATTIENI_TENSORE ---
    -> Incrementa il contatore dei riferimenti del tensore
    -> Evita copie fisiche dei dati (Es. durante le duplicazioni sullo stack).
*/
void trattieni_tensore(Tensore* t)
{
    if (t != NULL)
    {
        t -> contatore_rif++;  
    }
}


/* --- RILASCIA_TENSORE --- 
    -> Decrementa il contatore dei riferimenti del tensore;
    -> Dealloca la memoria (o chiude la mmap) se i riferimenti scendono a zero.
*/
void rilascia_tensore (Tensore* t)
{
    // 1. Controllo: Se il tensore e' NULL, ignoro l'operazione
    if (t == NULL)
    {
        return;
    }

    // 2. Decremento il contatore dei riferimenti
    t -> contatore_rif--;   

   // 3. Se il contatore arriva a zero, elimino definitivamente il tensore
    if (t -> contatore_rif == 0)
    {
        if (t -> mmap_attivo == 0)
        {
            // Dati allocati in RAM: libero usando free
            if (t -> dati != NULL)
            {
                free (t -> dati);
            }
        }
        else
        {
            // Dati mappati dal disco: chiudo la mappatura con munmap
            if (t -> dati != NULL)
            {
                // Torno indietro di 64 bite per fornire a munmap l'indirizzo di partenza corretto
                void* base_ptr = (void*)((char*)t -> dati - 64);
                munmap(base_ptr, t -> mmap_size);
            }
        }

        // 4. Libero il tensore
        free (t);
    }
}


/*  --- STAMPA_TENSORE ---
    -> Stampa a schermo il contenuto del tensore.
    -> Formattazione: Tensor (shape=[...], data=[...])
*/
void stampa_tensore(Tensore* t)
{
    // 1. Controlli di integrità del tensore
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

    // 2. Stampa formattata in base alle dimensioni
    if (t -> num_dim == 1)
    {
        // CASO VETTORE 1D
        printf("Tensor (shape=[%d], data=[", t -> forma[0]);
        for (int i = 0; i < t -> forma[0]; i++)
        {
            printf("%f%s", t -> dati[i], (i == t -> forma[0] - 1) ? "" : " ");
        }
        printf("])\n");
    }
    else if (t -> num_dim == 2)
    {
        // CASO MATRICE 2D
        printf("Tensor (shape=[%d %d], data=[\n", t -> forma[0], t -> forma[1]);
        int righe = t -> forma[0];
        int colonne = t -> forma[1];

        // Doppio ciclo per simulare la griglia spaziale
        for (int r = 0; r < righe; r++)
        {
            printf("  [ ");
            for (int c = 0; c < colonne; c++)
            {
                // Calcolo l'indice dell'array monodimensionale
                int indice = (r * colonne) + c;
                printf ("%f%s", t -> dati[indice], (c == colonne - 1) ? "" : " ");
            }
            printf("]%s\n", (r == righe - 1) ? "" : ",");
        }
        printf("])\n");
    }
    else
    {
        printf("Errore: Numero di dimensioni non supportato (%d).\n", t -> num_dim);
    }
    printf ("\n");          
}



/* ----------   PARTE 2: STACK     ---------- */

/*  --- CREA_STACK ---
    -> Alloca e inizializza uno stack vuoto
*/
Stack* crea_stack(void)
{
    // 1. Chiedo al sistema operativo spazio per la struttura principale dello Stack
    Stack* nuovo_stack = (Stack*) malloc(sizeof(Stack));

    // 2. Controllo errori di allocazione
    if (nuovo_stack == NULL)
    {
        perror ("Errore: Impossibile allocare memoria per lo Stack");
        exit (EXIT_FAILURE);
    }

    // 3. Inizializzo lo stack vuoto (il puntatore in cima è NULL)
    nuovo_stack -> cima = NULL;

    return nuovo_stack;
}

/*  --- PUSH ---
    -> Inserisce un tensore in cima allo stack
*/
void push(Stack*s, Tensore* t)
{
    // 1. Controllo validità dell'input
    if (s == NULL || t == NULL)
    {
        return;
    }

    // 2. Creo il nuovo nodo della catena
    NodoStack* nuovo_nodo = (NodoStack*)malloc(sizeof(NodoStack));

    if (nuovo_nodo == NULL)
    {
        perror ("Errore: Impossibile allocare memoria per un NodoStack");
        exit (EXIT_FAILURE);
    }

    // 3. Inserisco il dato e aggancio il nuovo nodo alla vecchia cima
    nuovo_nodo -> tensore = t;                  
    nuovo_nodo -> prossimo = s -> cima;         
    s -> cima = nuovo_nodo;
}

/*  --- POP ---
    -> Rimuove e restituisce il tensore in cima allo stack
*/
Tensore* pop (Stack* s)
{
    // 1. Controllo: Se lo stack è vuoto, segnalo l'errore ed esco
    if (s == NULL || s -> cima == NULL)
    {
        fprintf (stderr, "Errore: tentativo di estrarre un elemento da uno stack vuoto!\n");
        exit (EXIT_FAILURE);   
    }

    // 2. Individuo il nodo da rimuovere (l'attuale cima) e ne salvo il tensore
    NodoStack* nodo_da_rimuovere = s -> cima;
    Tensore* tensore_estratto = nodo_da_rimuovere -> tensore;

    // 3. Aggiorno la cima: La nuova cima diventa il nodo sottostante
    s -> cima = nodo_da_rimuovere -> prossimo;

    // 4. Distruggo fisicamente il vecchio nodo e restituisco il tensore
    free (nodo_da_rimuovere);
    return tensore_estratto;
}

/*  --- LIBERA_STACK ---
    -> Svuota completamente lo stack, rilasciando la memoria in modo sicuro
*/
void libera_stack(Stack* s)
{
    // 1. Controllo: Se lo stack non esiste, non c'è nulla da liberare
    if (s == NULL)
    {
        return;
    }

    // 2. Finchè la cima non è vuota, continuo a estrarre e distruggere
    while (s -> cima != NULL)
    {
        Tensore* t = pop(s);            
        rilascia_tensore(t);            
    }

    // 3. Libero la struttura principale
    free(s);
}



/* ----------   PARTE 3: MATEMATICA E OPERAZIONI   ---------- */

/*  --- SOMMA TENSORI (+) ---
    -> Applica l'operatore + su due tensori.
    -> Notazione: b a -- a+b
*/
void op_somma(Stack*s)
{
    // 1. Estrazione operandi. Notazione: b a -- a+b 
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
        totale_elementi *= a -> forma[i];           // Calcolo contestualmente quanti elementi ci sono
    }

    // 3. Allocazione del risultato
    Tensore* c = crea_tensore (a -> num_dim, a -> forma);

    // 4. Calcolo paralello
    #pragma omp parallel for                    // Dice al compilatore di dividere il ciclo for su più thread    
    for (int i = 0; i < totale_elementi; i++)
    {
        c -> dati[i] = a -> dati[i] + b -> dati[i];
    }

    // 5. Aggiornamento stack e  pulizia memoria
    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}

/*  --- SOTTRAZIONE TENSORI (-) ---
    -> Applica l'operatore - su due tensori.
    -> Notazione: b a -- a-b
*/
void op_sottrazione(Stack* s)
{
    Tensore* a = pop(s);
    Tensore* b = pop(s);

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

    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        c -> dati[i] = a -> dati[i] - b -> dati[i];
    }

    push (s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}

/*  --- PRODOTTO TENSORI (*) ---
    -> Applica l'operatore * su due tensori.
    -> Notazione: b a -- a*b
*/
void op_prodotto(Stack* s)
{
    Tensore* a = pop(s);
    Tensore* b = pop(s);

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

    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        c -> dati[i] = a -> dati[i] * b -> dati[i];
    }

    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}



/* ----------      PARTE 4: COMPARAZIONI, LOGICA E SELEZIONE    --------------- */

/* MINORE (<) 
    -> Confronto elemento per elemento (a < b). Restituisce 1.0 (Vero) o 0.0 (Falso).
    -> Notazione: b a -- a<b
*/
void op_minore(Stack* s)
{
   // 1. Estrazione operandi. Notazione: b a -- a<b
    Tensore* a = pop(s);
    Tensore* b = pop(s);

    // 2. Controllo compatibilità e forma
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

    // 5. Aggiornamento stack e pulizia
    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}

/*  MAGGIORE (>) 
    -> Confronta elemento per elemento (a > b).
    -> Inserisce 1.0 se vero, 0.0 se falso. 
    -> Notazione: b a -- a>b
*/
void op_maggiore(Stack* s)
{
    Tensore* a = pop(s);
    Tensore* b = pop(s);

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

    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        // Se (a > b) e' vero assegna 1.0f, altrimenti 0.0f
        c -> dati[i] = (a -> dati[i] > b -> dati[i]) ? 1.0f : 0.0f;
    }

    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}

/*  UGUALE (=) 
    -> Confronta elemento per elemento (a == b)
    -> Inserisce un nuovo tensore risultato contenente 1.0 (Vero) o 0.0 (Falso).
    -> Notazione: b a -- a=b
*/
void op_uguale(Stack* s)
{
    Tensore* a = pop(s);
    Tensore* b = pop(s);

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

    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        // Se (a == b) e' vero assegna 1.0f, altrimenti 0.0f
        c -> dati[i] = (a -> dati[i] == b -> dati[i]) ? 1.0f : 0.0f;
    }

    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}

/*  AND LOGICO (&) 
    -> Applica l'operatore logico AND elemento per elemento.
    -> I tensori in input devono contenere unicamente 0.0 e 1.0.
    -> Inserisce un nuovo tensore risultato contenente 1.0 o 0.0.
    -> Notazione: b a -- a&b
*/
void op_and(Stack* s)
{
    Tensore* a = pop(s);
    Tensore* b = pop(s);

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

    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        // Se sia 'a' che 'b' sono veri assegna 1.0f, altrimenti 0.0f
        c -> dati[i] = (a -> dati[i] != 0.0f && b -> dati[i] != 0.0f) ? 1.0f : 0.0f;
    }

    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}

/*  OR LOGICO (|) 
    -> Applica l'operatore logico OR elemento per elemento.
    -> Inserisce 1.0 se almeno uno dei due elementi e' vero (!= 0.0).
    -> Notazione: b a -- a|b
*/
void op_or(Stack* s)
{
    Tensore* a = pop(s);
    Tensore* b = pop(s);

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

    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        // OR logico: vero se 'a' o 'b' sono diversi da 0.0
        c -> dati[i] = (a -> dati[i] != 0.0f || b -> dati[i] != 0.0f) ? 1.0f : 0.0f;
    }

    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}

/*  NOT LOGICO (!) 
    -> Operazione unaria: Estrae un singolo tensore
    -> Inverte i valori logici (0.0 -> 1.0 e 1.0 -> 0.0).
    -> Notazione: a -- !a
*/
void op_not(Stack* s)
{
    Tensore* a = pop(s);
    
    int totale_elementi = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        totale_elementi *= a -> forma[i];
    }

    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        // Se 'a' è 0.0, diventa 1.0. Altrimenti diventa 0.0.
        c -> dati[i] = (a -> dati[i] == 0.0f) ? 1.0f : 0.0f;
    }

    push(s, c);
    rilascia_tensore(a);
}

/*   OPERATORE DI SELEZIONE ($) 
    -> Estrae tre tensori e agisce come un multiplexer 
    -> Richiede che tutti e tre i tensori abbiano dimensioni e forme identiche.
    -> Notazione: b a m -- m?a:b
*/
void op_selezione(Stack* s)
{
    Tensore* m = pop(s);    // 'm' è l'elemento in cima
    Tensore* a = pop(s);    // 'a' è il secondo
    Tensore* b = pop(s);    // 'b' è il terzo
    
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

    // Allocazione del risultato
    Tensore* c = crea_tensore(a -> num_dim, a -> forma);

    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        // Se m è diverso da 0.0 (Vero), seleziona l'elemento da 'a', altrimenti da 'b'
        c -> dati[i] = (m -> dati[i] != 0.0f) ? a -> dati[i] : b -> dati[i];
    }

    push(s, c);
    rilascia_tensore(m);
    rilascia_tensore(a);
    rilascia_tensore(b);
}



/* ----------   PARTE 5: MATEMATICA AVANZATA E STATISTICA    ---------- */

/* --- PRODOTTO DI MATRICI (@) ---
    -> Esegue la moltiplicazione matriciale riga per colonna (A @ B).
    -> Entrambi i tensori devono essere rigorosamente matrici 2D.
    -> Notazione: b a -- a@b
*/
void op_prodotto_matrici(Stack* s)
{
    // 1. Estrazione operandi. Notazione: b a -- a@b
    Tensore* a = pop(s);
    Tensore* b = pop(s);

    // 2. Controllo dimensioni (devono essere matrici 2D)
    if (a -> num_dim != 2 || b -> num_dim != 2)
    {
        fprintf(stderr, "Errore: Il prodotto di matrici (@) richiede due tensori 2D.\n");
        exit(EXIT_FAILURE);
    }

    // 3. Controllo algebra lineare: Colonne di A == righe di B. 
    if (a -> forma[1] != b -> forma[0])
    {
        fprintf(stderr, "Errore: Dimensioni interne incompatibili per il prodotto di matrici (@).\n");
        exit (EXIT_FAILURE);
    }

    // 4. Allocazione del tensore risultato C (dimensioni: righe di A x colonne di B)
    int32_t forma_c[2] = {a -> forma [0], b -> forma[1]};
    Tensore*c = crea_tensore(2, forma_c);

    // Salvo le dimensioni in variabli per comodità
    int righe_a = a -> forma[0];
    int col_a_righe_b = a -> forma[1];
    int col_b = b -> forma[1];

    // 5. Calcolo parallelo sulle righe di C
    #pragma omp parallel for
    for (int i = 0; i < righe_a; i++)
    {
        for (int j = 0; j < col_b; j++)
        {
            float somma = 0.0f;
            for (int k = 0; k < col_a_righe_b; k++)
            {
                // Mappatura da array 1D a griglia 2D: (riga * tot_colonne) + colonna
                float val_a = a -> dati[i * col_a_righe_b + k];
                float val_b = b -> dati[k * col_b + j];
                somma += val_a * val_b;
            }
            c -> dati [i * col_b + j] = somma; 
        }
    }

    // 6. Aggiornamento stack e pulizia
    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}

/* --- PRODOTTO INTERNO (.) ---
    -> Calcola il prodotto interno (dot product) tra due tensori identici per forma.
    -> Restituisce un tensore 1D contenente un singolo valore (scalare).
    -> Notazione: b a -- a.b
*/
void op_prodotto_interno(Stack* s)
{
    // 1. Estrazione operandi
    Tensore* a = pop(s);
    Tensore* b = pop(s);

    // 2. Verifica compatibilità strutturale (devono essere vettori 1D)
    if (a -> num_dim != 1 ||  b -> num_dim != 1)
    {
        fprintf(stderr, "Errore: Il prodotto interno (.) richiede due tensori 1D (vettori).\n");
        exit(EXIT_FAILURE);
    }
    if (a -> forma[0] != b -> forma[0])
    {
        fprintf(stderr, "Errore: Forma dei tensori incompatibile per il prodotto interno (.).\n");
        exit(EXIT_FAILURE);
    }

    // 3. Calcolo della riduzione con clausola reduction per evitare race condition su thread
    float somma_totale = 0.0f;
    int totale_elementi = a -> forma[0];

    #pragma omp parallel for reduction (+: somma_totale)
    for (int i = 0; i < totale_elementi; i++)
    {
        somma_totale += a -> dati[i] * b -> dati[i];
    }

    // 4. Allocazione e salvataggio del risultato scalare
    int32_t forma_c[1] = {1};
    Tensore* c = crea_tensore(1, forma_c);
    c -> dati[0] = somma_totale;

    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b);
}

/* --- CONVOLUZIONE 2D (c) ---
    -> Esegue la convoluzione tra una matrice immagine 'a' e un kernel 'k'.
    -> Applica zero-padding ai bordi per mantenere il risultato della stessa forma di 'a'.
    -> Notazione: a k -- conv(a, k)
*/
void op_convoluzione_2d(Stack* s)
{
    // 1. Estrazione operandi
    Tensore* k = pop(s);     // 'k' è il kernel in cima
    Tensore* a = pop(s);    // 'a' è l'immagine sotto

    // 2. Verifica delle dimensioni (Entrambi devono essere 2D) 
    if (a -> num_dim != 2 || k -> num_dim != 2)
    {
        fprintf(stderr, "Errore: La convulzione 2D (c) richiede due tensori 2D.\n");
        exit (EXIT_FAILURE);
    }

    int righe_a = a -> forma[0];
    int col_a = a -> forma[1];
    int righe_k = k -> forma[0];
    int col_k = k -> forma[1];

    // 3. Allocazione matrice risultato e calcolo degli offset per centrare il kernel
    Tensore*c = crea_tensore(2, a -> forma);
    int offset_r = righe_k / 2;
    int offset_c = col_k / 2;

    // 4. Applicazione della paralelizzazione sull'immagine
    #pragma omp parallel for 
    for (int i = 0; i < righe_a; i++)
    {
        for (int j = 0; j < col_a; j++)
        {
            float somma = 0.0f;

            // Faccio scorrere il kernel
            for (int ki = 0; ki < righe_k; ki++)
            {
                for (int kj = 0; kj < col_k; kj++)
                {
                    // Calcolo le coordinate reali sull'immagine originale
                    int riga_orig = i + ki - offset_r;
                    int col_orig = j + kj - offset_c;

                    float val_a = 0.0f;

                    // Zero - padding: prelevo il valore selo se le cordinate sono dentro i bordi
                    if (riga_orig >= 0 && riga_orig < righe_a && col_orig >= 0 && col_orig < col_a)
                    {
                        val_a = a -> dati[riga_orig * col_a + col_orig];
                    }

                    float val_k = k -> dati[ki * col_k + kj];
                    somma += val_a * val_k;
                }
            }
            c -> dati[i * col_a + j] = somma;
        }
    }

    // 5. Inserimento del risultato e pulizia
    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(k);
}



/* ----------   // PARTE 6: FORMA DEI TENSORI   ---------- */

/* --- RAVEL (~) ---
    -> Appiattisce un tensore trasformandolo in un vettore 1D.
    -> Modifica i metadati in-place senza riallocare i dati fisici.
    -> Notazione: a -- a'
*/
void op_ravel(Stack* s)
{
    // 1. Estrazione operando
    Tensore* a = pop(s);

    // 2. Se e' gia' 1D, lo rimetto nello stack e termino
    if (a -> num_dim == 1)
    {
        push(s, a);
        return;
    }

    // 3. Calcolo il totale degli elementi fisici
    int totale_elementi = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        totale_elementi *= a -> forma[i];
    }

    // 4. Modifico la forma direttamente nel tensore esistente (in-place)
    a -> num_dim = 1;
    a -> forma[0] = totale_elementi;
    a -> forma[1] = 0; // Pulizia per sicurezza

    // 5. Reinserimento del tensore aggiornato
    push(s, a);
}

/* --- SHAPE (#) ---
    -> Estrae le dimensioni del tensore in cima allo stack (il tensore viene consumato).
    -> Restituisce un vettore 1D contenente i valori della forma originale convertiti in float.
    -> Notazione: a -- #a
*/
void op_shape(Stack* s)
{
    // 1. Estrazione operandi
    Tensore* a = pop(s);

    // 2. Allocazione del risultato: un vettore 1D lungo quanto il numero di dimensioni di 'a'
    int32_t forma_c[1] = { a -> num_dim };
    Tensore* c = crea_tensore(1, forma_c);

    // 3. Copia dei valori strutturali (convertiti in float) nel nuovo tensore
    for (int i = 0; i < a -> num_dim; i++)
    {
        c -> dati[i] = (float) a -> forma[i];
    }
    
    // 4. Inserimento e pulizia
    push(s, c);
    rilascia_tensore(a);
}

/* --- RESHAPE (r) ---
    -> Modifica le dimensioni di un tensore in-place (senza alterare la memoria fisica).
    -> Il numero totale di elementi deve rimanere invariato.
    -> Notazione: a s -- a'
*/
void op_reshape(Stack* s)
{
    // 1. Estrazione operandi
    Tensore* s_shape = pop(s); // Vettore 1D con la nuova forma
    Tensore* a = pop(s);      // Tensore da modificare

    // 2. Controlli strutturali: la nuova forma deve essere un vettore 1D
    if (s_shape -> num_dim != 1)
    {
        fprintf(stderr, "Errore: Il tensore della forma per reshape (r) deve essere in 1D. \n");
        exit(EXIT_FAILURE); 
    }

    // 3. Calcolo degli elementi totali originali
    int elementi_a = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        elementi_a *= a -> forma[i];
    }

    // 4. Calcolo degli elementi totali previsti dalla nuova forma
    int elementi_nuovi = 1;
    for (int i = 0; i < s_shape -> forma[0]; i++)
    {
        elementi_nuovi *= (int)s_shape -> dati[i];
    }

    // 5. Verifica compatibilità: i dati fisici non devono variare in numero
    if (elementi_a != elementi_nuovi)
    {
        fprintf(stderr, "Errore: Numero totale di elementi incompatibile per il reshape (r).\n");
        exit(EXIT_FAILURE);
    }

    // 6. Aggiornamento in-place dei metadati dimensionali
    a -> num_dim = s_shape -> forma[0];
    for (int i = 0; i < a -> num_dim; i++)
    {
        a -> forma[i] = (int)s_shape -> dati[i];
    }

    // 7. Inserimento e pulizia
    push (s, a);
    rilascia_tensore(s_shape);
}



/* ----------   PARTE 7: GENERAZIONE CASUALE   ---------- */

/* --- GENERAZIONE CASUALE (?) ---
    -> Crea un tensore con dimensioni indicate dal vettore 's', riempito di valori casuali [0, 1].
    -> Il calcolo NON e' parallelizzato perche' rand() non e' thread-safe.
    -> Notazione: s -- a
*/
void op_random(Stack* s)
{
    // 1. Estrazione operando che funge da guida per le dimensioni a
    Tensore* s_shape = pop(s);

    // 2. Controlli di sicurezza sulla validita' del tensore guida
    if (s_shape -> num_dim != 1)
    {
        fprintf(stderr, "Errore: Il tensore delle forma per la generazione casuale (?) deve essere 1D.\n");
        exit(EXIT_FAILURE);  
    }
    if (s_shape -> forma[0] > MAX_DIM || s_shape -> forma[0] == 0)
    {
        fprintf(stderr, "Errore: Numero di dimensioni non valido per la generazione casuale (Max %d).\n", MAX_DIM);
        exit(EXIT_FAILURE);
    }

    // 3. Estrapolazione delle dimensioni finali e del totale elementi
    int32_t nuova_forma[MAX_DIM];
    int num_dim_nuovo = s_shape -> forma[0];
    int totale_elementi = 1;

    for(int i = 0; i < num_dim_nuovo; i++)
    {
        nuova_forma[i] = (int) s_shape -> dati[i];
        totale_elementi *= nuova_forma[i];
    }

    // 4. Creazione del nuovo tensore
    Tensore* a = crea_tensore(num_dim_nuovo, nuova_forma);

    // 5. Popolamento seriale con numeri casuali normalizzati tra 0.0 e 1.0
    for(int i = 0; i < totale_elementi; i++)
    {
        a -> dati[i] = (float)rand() / (float)RAND_MAX;
    }

    // 6. Inserimento e pulizia del tensore guida
    push(s, a);
    rilascia_tensore(s_shape);
}



/* ----------   PARTE 8: OPERAZIONI ELEMENTO PER ELEMENTO, RIDUZIONE E FILLING   ---------- */

/* --- RELU (R) ---
    -> Sostituisce tutti gli elementi negativi con 0.0 (relu(x) = max(0, x)).
    -> L'operazione viene eseguita in-place per ottimizzare la memoria.
    -> Notazione: a -- relu(a)
*/
void op_relu(Stack* s)
{
    // 1. Estrazione operando
    Tensore* a = pop(s);

    // 2. Calcolo totale elementi
    int totale_elementi = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        totale_elementi *= a -> forma[i];
    }

    // 3. Modifica in-place (Parallelizzata)
    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        if (a -> dati[i] < 0.0f)
        {
            a -> dati[i] = 0.0f;
        }
    }

    // 4. Reinserimento del tensore aggiornato
    push (s, a);
}

/* --- MINIMO (m) ---
    -> Ritorna elemento per elemento il minimo tra a e b.
    -> I tensori in input devono avere dimensioni identiche.
    -> Notazione: b a -- min(a,b)
*/
void op_minimo(Stack* s)
{
    // 1. Estrazione operandi
    Tensore* a = pop(s);
    Tensore* b = pop(s);

    // 2. Controllo compatibilità strutturale
    if (a -> num_dim != b -> num_dim)
    {
        fprintf(stderr, "Errore: Numero di dimensioni incompatibili per il minimo (m).\n");
        exit (EXIT_FAILURE);
    }

    int totale_elementi = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        if (a -> forma[i] != b -> forma[i])
        {
            fprintf(stderr, "Errore: Forma dei tensori incompatibili per il minimo (m).\n");
            exit(EXIT_FAILURE);
        }
        totale_elementi *= a -> forma[i];
    }

    // 3. Allocazione risultato
    Tensore* c = crea_tensore (a -> num_dim, a -> forma);

    // 4. Calcolo paralellizzato (assegnazione del minimo)
    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        c -> dati[i] = (a -> dati[i] < b -> dati[i]) ? a -> dati[i] : b -> dati[i];
    }

    // 5. Inserimento e pulizia
    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b); 
}

/* --- MASSIMO (M) ---
    -> Ritorna elemento per elemento il massimo tra a e b.
    -> Notazione: b a -- max(a,b)
*/
void op_massimo(Stack* s)
{
    Tensore* a = pop(s);
    Tensore* b = pop(s);

    if (a -> num_dim != b -> num_dim)
    {
        fprintf(stderr, "Errore: Numero di dimensioni incompatibili per il massimo (M).\n");
        exit (EXIT_FAILURE);
    }

    int totale_elementi = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        if (a -> forma[i] != b -> forma[i])
        {
            fprintf(stderr, "Errore: Forma dei tensori incompatibili per il massimo (M).\n");
            exit(EXIT_FAILURE);
        }
        totale_elementi *= a -> forma[i];
    }

    Tensore* c = crea_tensore (a -> num_dim, a -> forma);

    #pragma omp parallel for
    for (int i = 0; i < totale_elementi; i++)
    {
        c -> dati[i] = (a -> dati[i] > b -> dati[i]) ? a -> dati[i] : b -> dati[i];
    }

    push(s, c);
    rilascia_tensore(a);
    rilascia_tensore(b); 
}



/* ----------   PARTE 9: OPERAZIONI DI RIDUZIONE E FILLING   ---------- */

/* --- RIDUZIONE (S) ---
    -> Ritorna la somma di tutti i valori presenti nel tensore.
    -> Restituisce il risultato come un vettore 1D di un singolo elemento (scalare).
    -> Notazione: a -- S(a)
*/
void op_somma_riduzione(Stack* s)
{
    // 1. Estrazione operando
    Tensore* a = pop(s);

    // 2. Calcolo elementi totali
    int totale_elementi = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        totale_elementi *= a -> forma[i]; 
    }

    // 3. Somma sequenziale per garantire il determinismo nei test di uguaglianza (No OpenMP)
    float somma_totale = 0.0f;
    for (int i = 0; i < totale_elementi; i++)
    {
        somma_totale += a -> dati[i];
    }

    // 4. Creazione del nuovo tensore scalare (1D, 1 elemento)
    int32_t forma_c[1] = {1};
    Tensore* c = crea_tensore(1, forma_c);
    c -> dati[0] = somma_totale;

    // 5. Inserimento e pulizia
    push(s, c);
    rilascia_tensore(a);
}

/* --- FILL (f) ---
    -> Crea un tensore di dimensioni indicate da 's' riempito con i valori di 'v' ripetuti.
    -> Sfrutta l'aritmetica modulare per ripetere ciclicamente i valori originali.
    -> Notazione: s v -- a
*/
void op_fill(Stack* s)
{
    // 1. Estrazione operandi
    Tensore* v = pop(s);            // 'v' valori in cima  
    Tensore* s_shape = pop(s);     // 's-shape' è la forma sotto

    // 2. Controlli di sicurezza sulla validita' della forma
    if (s_shape -> num_dim != 1)
    {
        fprintf(stderr, "Errore: Il tensore della forma per il fill (f) deve essere 1D.\n");
        exit(EXIT_FAILURE);
    }
    if (s_shape -> forma[0] > MAX_DIM || s_shape -> forma[0] == 0)
    {
        fprintf(stderr, "Errore: Numero di dimensioni non valido per il fill (Max %d).\n", MAX_DIM);
        exit(EXIT_FAILURE);
    }

    // 3. Calcolo del totale degli elementi della nuova griglia e allocazione forma
    int totale_elementi_nuovi = 1;
    int32_t nuova_forma[MAX_DIM];
    for (int i = 0; i < s_shape -> forma[0]; i++)
    {
        nuova_forma[i] = (int)s_shape -> dati[i];
        totale_elementi_nuovi *= nuova_forma[i];
    }

    // 4. Calcolo degli elementi totali del vettore origine
    int totale_elementi_v = 1;
    for(int i = 0; i < v -> num_dim; i++)
    {
        totale_elementi_v *= v -> forma[i];
    }

    // 5. Creazione del nuovo tensore e riempimento parallelizzato con ripetizione modulare
    Tensore* c = crea_tensore(s_shape -> forma[0], nuova_forma);

    #pragma omp parallel for
    for (int i = 0; i < totale_elementi_nuovi; i++)
    {
        c -> dati[i] = v -> dati[i % totale_elementi_v];
    }

    // 6. Inserimento e pulizia
    push(s, c);
    rilascia_tensore(s_shape);
    rilascia_tensore(v);
}



/* ----------   PARTE 10: UTILITA' E MANIPOLAZIONI DELLO STACK   ---------- */

/* --- STAMPA (p) ---
    -> Estrae il tensore in cima allo stack, lo stampa a video e lo rilascia.
    -> Notazione: a -- 
*/
void op_stampa(Stack* s)
{
    // 1. Estrazione operando
    Tensore* a = pop(s); 

    // 2. Visualizzo il contenuto 
    stampa_tensore(a);  
    
    // 3. Dealloco la memoria    
    rilascia_tensore(a);    
}

/* --- DUPLICAZIONE (d) ---
    -> Duplica l'elemento in cima allo stack.
    -> Ottimizzazione: non copia i dati fisici, incrementa solo il reference counter.
    -> Notazione: a -- a a
*/
void op_duplica(Stack* s)
{
    // 1. Controllo sicurezza
    if (s == NULL || s -> cima == NULL)
    {
        fprintf(stderr, "Errore: Impossibile duplicare (d), stack vuoto.\n");
        exit(EXIT_FAILURE);
    }

    // 2. Leggo il tensore in cima senza estrarlo
    Tensore* a = s -> cima -> tensore;

    // 3. Incremento il contatore dei riferimenti
    trattieni_tensore(a);

    // 4. Inserisco un nuovo nodo nello stack che punta allo stesso tensore
    push(s, a);
}

/* --- SCAMBIO (s) ---
    -> Scambia di posizione i due elementi in cima allo stack.
    -> Notazione: b a -- a b
*/
void op_scambia(Stack* s)
{
    // 1. Estrazione
    Tensore* a = pop(s);
    Tensore* b = pop(s);

    // 2. Reinserimento in ordine inverso
    push(s, a);
    push(s, b);
}

/* --- DROP (D) ---
    -> Rimuove e distrugge l'elemento in cima allo stack.
    -> Notazione: a -- 
*/
void op_drop(Stack* s)
{
    Tensore* a = pop(s);
    rilascia_tensore(a);
}

/* --- OVER (o) ---
    -> Duplica il secondo elemento dello stack e lo mette in cima.
    -> Ottimizzazione: come 'd', incrementa solo il reference counter.
    -> Notazione: a b -- a b a
*/
void op_over(Stack* s)
{
    // 1. Controllo sicurezza (servono almeno due elementi)
    if (s == NULL || s -> cima == NULL || s -> cima -> prossimo == NULL)
    {
        fprintf(stderr, "Errore: Impossibile eseguire over (o), stack con meno di due elementi .\n");
        exit(EXIT_FAILURE);
    }

    // 2. Leggo il secondo tensore
    Tensore* a = s -> cima -> prossimo -> tensore;

    // 3. Incremento il contatore dei riferimenti di 'a'
    trattieni_tensore(a);

    // 4. Inserisco il nuovo riferimento in cima allo stack
    push(s, a);
}


/* ----------   PARTE 11: INPUT/OUTPUT   ---------- */

/* --- LETTURA PGM '(' ---
    -> Legge un'immagine PGM in formato testo (scala di grigi).
    -> La carica nello stack come matrice 2D.
    -> Normalizza i pixel: trasforma i numeri da 0-255 in float da 0.0 a 1.0.
    -> Notazione: filename -- tensor
*/
void op_leggi_pgm(Stack* s, const char* nome_file)
{
    // 1. Apertura file in modalità lettura binaria ("rb")
    FILE* file = fopen(nome_file, "rb");
    if (!file)
    {
        fprintf(stderr, "Errore: Impossibile aprire il file %s.\n", nome_file);
        exit(EXIT_FAILURE);
    }

    char formato[3];
    int colonne, righe, max_val;

    // 2. Lettura dell'intestazione tipica dei file PGM 
    fscanf(file, "%2s", formato); 
    fscanf(file, "%d %d", &colonne, &righe);
    fscanf(file, "%d", &max_val);
    fgetc(file);            // Salta il singolo carattere di whitespace dopo max_val

    // 3. Creazione del tensore vuoto in RAM per ospitare l'immagine
    int32_t forma[2] = {righe, colonne};
    Tensore* img = crea_tensore(2, forma);

    // 4. Lettura binaria dei pixel (1 byte per pixel) e normalizzazione
    unsigned char* pixel_buffer = (unsigned char*)malloc(righe * colonne);
    fread(pixel_buffer, 1, righe * colonne, file);
    
    for (int i = 0; i < righe * colonne; i++)
    {
        img -> dati[i] = (float)pixel_buffer[i] / (float)max_val;
    }

    // 5. Chiusura del file, pulizia buffer temporaneo e inserimento in cima allo stack
    free(pixel_buffer);
    fclose(file);
    push(s, img);
}

/* --- SCRITTURA PGM ')' ---
    -> Salva un tensore 2D (presente in cima allo stack) come immagine PGM.
    -> Denormalizza i pixel: riporta i float (0.0-1.0) al formato intero (0-255).
    -> Notazione: a filename --
*/
void op_scrivi_pgm(Stack* s, const char* nome_file)
{
    // 1. Estrazione operando e controllo della forma (deve essere un'immagine 2D)
    Tensore* img = pop(s);
    if (img -> num_dim != 2)
    {
        fprintf(stderr, "Errore: La scrittura PGM richiede un tensore 2D.\n");
        exit(EXIT_FAILURE);
    }

    // 2. Apertura del file in modalità scrittura testo ("wb")
    FILE* file = fopen(nome_file, "wb");
    if (!file)
    {
        fprintf(stderr, "Errore: Impossibile aprire il file %s.\n", nome_file);
        exit(EXIT_FAILURE);
    }

    int righe = img -> forma[0];
    int colonne = img -> forma[1];

    // 3. Scrittura manuale dell'intestazione del formato PGM
    fprintf(file, "P5\n%d %d\n255\n", colonne, righe);

    // 4. Ciclo di denormalizzazione e scrittura su file binaria
    unsigned char* pixel_buffer = (unsigned char*)malloc(righe * colonne);
    for (int i = 0; i < righe * colonne; i++)
    {
        // Controllo: se un calcolo ha generato valori anomali, li forzo nel range 0.0-1.0
        float val = img->dati[i];
        if (val < 0.0f) val = 0.0f;
        if (val > 1.0f) val = 1.0f;
        pixel_buffer[i] = (unsigned char)(val * 255.0f);
        
    }
    fwrite(pixel_buffer, 1, righe * colonne, file);

    // 5. Chiusura file e pulizia memoria
    fclose(file);
    rilascia_tensore(img);
}

/* --- LETTURA BINARIA MAPPATA '{' ---
    -> Mappa in memoria l'intero file binario senza copiarne i dati in RAM.
    -> Legge e decodifica l'intestazione di 64 byte richiesta dal PDF del professore.
    -> Notazione: filename -- tensor
*/
void op_leggi_binario(Stack* s, const char* nome_file)
{
    // 1. Apertura del file a basso livello (file descriptor) per abilitare mmap
    int fd = open(nome_file, O_RDONLY);
    if (fd == -1)
    {
        perror("Errore apertura file binario");
        exit(EXIT_FAILURE);
    }

    // 2. Scopro dal sistema operativo la dimensione esatta del file in byte
    struct stat st;
    fstat(fd, &st);
    size_t fs = st.st_size;

    // 3. Mappo tutto il file in memoria (intestazione inclusa)
    void* mapped = mmap(NULL, fs, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) 
    { 
        perror("Errore mmap"); 
        exit(EXIT_FAILURE);
    }
    close(fd);

    // 4. Lettura dei metadati dai primi 64 byte 
    struct on_disk_tensor* hdr = (struct on_disk_tensor*)mapped;
    
    // 5. Creazione della struttura base del Tensore
    Tensore* t = (Tensore*)malloc(sizeof(Tensore));
    t -> num_dim = hdr -> ndim;
    
    // Copio la forma esatta dall'intestazione mappata
    for (int i = 0; i < t -> num_dim; i++) 
    { 
        t -> forma[i] = hdr -> shape[i]; 
    }
    
    t -> contatore_rif = 1;
    t -> mmap_attivo = 1; 
    t -> mmap_size = fs;

    // 6. I veri numeri iniziano all'offset indicato dalla struct
    t -> dati = (float*)((char*)mapped + hdr -> data_offset);

    push(s, t);
}

/* --- SCRITTURA BINARIA '}' ---
    -> Salva il tensore su disco costruendo un file binario allineato a 64 byte.
    -> Scrive la 'struct on_disk_tensor' nei primi 64 byte, seguita dai dati grezzi.
    -> Notazione: a filename --
*/
void op_scrivi_binario(Stack* s, const char* nome_file)
{
    // 1. Estrazione operando 
    Tensore* t = pop(s);

    // 2. Apertura del file in modalità scrittura binaria grezza (wb)
    FILE* f = fopen(nome_file, "wb");
    if (!f)
    {
        fprintf(stderr, "Errore: Impossibile creare il file binario %s.\n", nome_file);
        exit(EXIT_FAILURE);
    }

    // 3. Preparazione dell'intestazione
    struct on_disk_tensor hdr;
    memset(&hdr, 0, sizeof(struct on_disk_tensor));

    hdr.ndim = t -> num_dim;
    for (int i = 0; i < t -> num_dim; i++) 
    { 
        hdr.shape[i] = t -> forma[i]; 
    }     
    hdr.data_offset = 64; // I dati inizieranno esattamente al byte 64

    // 4. Scrittura massiva su disco
    char buffer[64] ={0};
    memcpy(buffer, &hdr, sizeof(struct on_disk_tensor));
    
    // Scrivo l'intestazione una sola volta (64 byte esatti)
    fwrite(buffer, 1, 64, f);

    // Calcolo quanti float devo scrivere in totale
    size_t totale_elementi = 1;
    for (int i = 0; i < t -> num_dim; i++) 
    { 
        totale_elementi *= t -> forma[i]; 
    }
    
    // Scrivo tutti i dati float in un colpo solo
    fwrite(t -> dati, sizeof(float), totale_elementi, f);

    // 5. Chiusura e pulizia
    fclose(f);
    rilascia_tensore(t);
}























