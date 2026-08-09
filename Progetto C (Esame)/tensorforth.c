// Alessio Matessi  S266112

// TensorForth: motore dei tensori e delle operazioni supportate.
// Questa implementazione gestisce:
//  - la creazione, la copia e la distruzione dei tensori
//  - le operazioni aritmetiche e logiche su tensori 1D/2D
//  - reshape, ravel, convoluzione e prodotto matriciale
//  - I/O PGM e I/O binario via mmap

#include "tensorforth.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static int read_pgm_token(FILE* file, char* buffer, size_t size)
{
    int ch;

    while (1)
    {
        ch = fgetc(file);
        if (ch == EOF)
        {
            return 0;
        }
        if (ch == '#')
        {
            while ((ch = fgetc(file)) != EOF && ch != '\n');
            continue;
        }
        if (!isspace(ch))
        {
            break;
        }
    }

    size_t idx = 0;
    do
    {
        if (idx + 1 < size)
        {
            buffer[idx++] = (char)ch;
        }
        ch = fgetc(file);
    } while (ch != EOF && !isspace(ch));

    buffer[idx] = '\0';
    return 1;
}

static int valid_shape_tensor(const Tensore* shape_tensor, int32_t* out_shape, int32_t* out_num_dim, size_t* out_total_elements)
{
    if (shape_tensor == NULL || shape_tensor->num_dim != 1)
    {
        return 0;
    }

    int32_t num_dim = shape_tensor->forma[0];
    if (num_dim <= 0 || num_dim > MAX_DIM)
    {
        return 0;
    }

    size_t totale = 1;
    for (int i = 0; i < num_dim; i++)
    {
        float raw_dim = shape_tensor->dati[i];
        if (raw_dim <= 0.0f)
        {
            return 0;
        }

        int32_t dim = (int32_t)raw_dim;
        float diff = raw_dim - (float)dim;
        if (diff > 1e-6f || diff < -1e-6f)
        {
            return 0;
        }

        if (dim <= 0)
        {
            return 0;
        }

        if (totale > SIZE_MAX / (size_t)dim)
        {
            return 0;
        }
        totale *= (size_t)dim;
        out_shape[i] = dim;
    }

    *out_num_dim = num_dim;
    if (out_total_elements != NULL)
    {
        *out_total_elements = totale;
    }

    return 1;
}

/* ----------   PARTE 1: TENSORI    ---------- */

/*  --- CREA_TENSORE ---
    -> Alloca dinamicamente un nuovo tensore e la relativa area di memoria per i dati;
    -> Inizializza il contatore dei riferimenti a 1
*/
Tensore* crea_tensore (int32_t num_dim, int32_t* forma)
{
    if (num_dim <= 0 || num_dim > MAX_DIM)
    {
        fprintf(stderr, "Errore: Numero di dimensioni non valido per il tensore (%d).\n", num_dim);
        exit(EXIT_FAILURE);
    }

    Tensore* nuovo_tensore = (Tensore*)malloc(sizeof(Tensore));
    if (nuovo_tensore == NULL)
    {
        perror ("Errore: Impossibile allocare memoria per la struttura del Tensore");
        exit (EXIT_FAILURE);
    }

    size_t totale_elementi = 1;
    nuovo_tensore -> num_dim = num_dim;

    for (int i = 0; i < num_dim; i++)
    {
        if (forma[i] <= 0)
        {
            fprintf(stderr, "Errore: Dimensione del tensore non valida (forma[%d] = %d).\n", i, forma[i]);
            free(nuovo_tensore);
            exit(EXIT_FAILURE);
        }

        if (totale_elementi > SIZE_MAX / (size_t)forma[i])
        {
            fprintf(stderr, "Errore: Dimensione del tensore troppo grande da allocare.\n");
            free(nuovo_tensore);
            exit(EXIT_FAILURE);
        }

        nuovo_tensore -> forma[i] = forma[i];
        totale_elementi *= (size_t)forma[i];
    }

    nuovo_tensore -> dati = (float*)calloc(totale_elementi, sizeof (float));
    if (nuovo_tensore -> dati == NULL)
    {
        perror ("Errore: Impossibile allocare memoria per i dati del Tensore");
        free (nuovo_tensore);
        exit (EXIT_FAILURE);
    }

   nuovo_tensore -> contatore_rif = 1;
   nuovo_tensore -> mmap_attivo = 0;
   nuovo_tensore -> mmap_size = 0;
   nuovo_tensore -> mmap_ptr = NULL;
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
    if (t == NULL)
    {
        return;
    }

    t -> contatore_rif--;
    if (t -> contatore_rif == 0)
    {
        if (t -> mmap_attivo == 0)
        {
            if (t -> dati != NULL)
            {
                free (t -> dati);
            }
        }
        else
        {
            if (t -> mmap_ptr != NULL && t -> mmap_size > 0)
            {
                munmap(t -> mmap_ptr, t -> mmap_size);
            }
        }

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
    Tensore* s_shape = pop(s);
    Tensore* a = pop(s);

    int32_t nuova_forma[MAX_DIM];
    int32_t num_dim_nuovo;
    size_t elementi_nuovi;

    if (!valid_shape_tensor(s_shape, nuova_forma, &num_dim_nuovo, &elementi_nuovi))
    {
        fprintf(stderr, "Errore: Forma non valida per reshape (r).\n");
        exit(EXIT_FAILURE);
    }

    size_t elementi_a = 1;
    for (int i = 0; i < a -> num_dim; i++)
    {
        elementi_a *= (size_t)a -> forma[i];
    }

    if (elementi_a != elementi_nuovi)
    {
        fprintf(stderr, "Errore: Numero totale di elementi incompatibile per il reshape (r).\n");
        exit(EXIT_FAILURE);
    }

    a -> num_dim = num_dim_nuovo;
    for (int i = 0; i < num_dim_nuovo; i++)
    {
        a -> forma[i] = nuova_forma[i];
    }

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
    Tensore* s_shape = pop(s);

    int32_t nuova_forma[MAX_DIM];
    int32_t num_dim_nuovo;
    size_t totale_elementi;

    if (!valid_shape_tensor(s_shape, nuova_forma, &num_dim_nuovo, &totale_elementi))
    {
        fprintf(stderr, "Errore: Forma non valida per la generazione casuale (?).\n");
        exit(EXIT_FAILURE);
    }

    Tensore* a = crea_tensore(num_dim_nuovo, nuova_forma);
    for (size_t i = 0; i < totale_elementi; i++)
    {
        a -> dati[i] = (float)rand() / (float)RAND_MAX;
    }

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
    Tensore* v = pop(s);
    Tensore* s_shape = pop(s);

    int32_t nuova_forma[MAX_DIM];
    int32_t num_dim_nuovo;
    size_t totale_elementi_nuovi;

    if (!valid_shape_tensor(s_shape, nuova_forma, &num_dim_nuovo, &totale_elementi_nuovi))
    {
        fprintf(stderr, "Errore: Forma non valida per il fill (f).\n");
        exit(EXIT_FAILURE);
    }

    size_t totale_elementi_v = 1;
    for (int i = 0; i < v -> num_dim; i++)
    {
        totale_elementi_v *= (size_t)v -> forma[i];
    }

    if (totale_elementi_v == 0)
    {
        fprintf(stderr, "Errore: Il tensore dei valori per il fill (f) non può essere vuoto.\n");
        exit(EXIT_FAILURE);
    }

    Tensore* c = crea_tensore(num_dim_nuovo, nuova_forma);

    #pragma omp parallel for
    for (size_t i = 0; i < totale_elementi_nuovi; i++)
    {
        c -> dati[i] = v -> dati[i % totale_elementi_v];
    }

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
    FILE* file = fopen(nome_file, "rb");
    if (!file)
    {
        fprintf(stderr, "Errore: Impossibile aprire il file %s.\n", nome_file);
        exit(EXIT_FAILURE);
    }

    char formato[16];
    char token[64];
    int colonne, righe, max_val;

    if (!read_pgm_token(file, formato, sizeof(formato)) || strcmp(formato, "P5") != 0)
    {
        fprintf(stderr, "Errore: Formato PGM non valido o file non supportato (%s).\n", nome_file);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if (!read_pgm_token(file, token, sizeof(token)) || sscanf(token, "%d", &colonne) != 1 || colonne <= 0)
    {
        fprintf(stderr, "Errore: Colonne non valide nel file PGM %s.\n", nome_file);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if (!read_pgm_token(file, token, sizeof(token)) || sscanf(token, "%d", &righe) != 1 || righe <= 0)
    {
        fprintf(stderr, "Errore: Righe non valide nel file PGM %s.\n", nome_file);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if (!read_pgm_token(file, token, sizeof(token)) || sscanf(token, "%d", &max_val) != 1 || max_val <= 0 || max_val > 255)
    {
        fprintf(stderr, "Errore: Valore massimo pixel non valido nel file PGM %s.\n", nome_file);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    int32_t forma[2] = {righe, colonne};
    Tensore* img = crea_tensore(2, forma);

    size_t totale_pixel = (size_t)righe * (size_t)colonne;
    unsigned char* pixel_buffer = (unsigned char*)malloc(totale_pixel);
    if (pixel_buffer == NULL)
    {
        perror("Errore: Impossibile allocare memoria per il buffer PGM");
        fclose(file);
        rilascia_tensore(img);
        exit(EXIT_FAILURE);
    }

    size_t read_count = fread(pixel_buffer, 1, totale_pixel, file);
    if (read_count != totale_pixel)
    {
        fprintf(stderr, "Errore: Lettura incompleta dei pixel PGM (%zu/%zu) nel file %s.\n", read_count, totale_pixel, nome_file);
        free(pixel_buffer);
        fclose(file);
        rilascia_tensore(img);
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < totale_pixel; i++)
    {
        img->dati[i] = (float)pixel_buffer[i] / (float)max_val;
    }

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
    Tensore* img = pop(s);
    if (img -> num_dim != 2)
    {
        fprintf(stderr, "Errore: La scrittura PGM richiede un tensore 2D.\n");
        exit(EXIT_FAILURE);
    }

    FILE* file = fopen(nome_file, "wb");
    if (!file)
    {
        fprintf(stderr, "Errore: Impossibile aprire il file %s.\n", nome_file);
        exit(EXIT_FAILURE);
    }

    int righe = img -> forma[0];
    int colonne = img -> forma[1];

    fprintf(file, "P5\n%d %d\n255\n", colonne, righe);

    size_t totale_pixel = (size_t)righe * (size_t)colonne;
    unsigned char* pixel_buffer = (unsigned char*)malloc(totale_pixel);
    if (pixel_buffer == NULL)
    {
        perror("Errore: Impossibile allocare memoria per il buffer PGM");
        fclose(file);
        rilascia_tensore(img);
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < totale_pixel; i++)
    {
        float val = img->dati[i];
        if (val < 0.0f) val = 0.0f;
        if (val > 1.0f) val = 1.0f;
        pixel_buffer[i] = (unsigned char)(val * 255.0f);
    }

    size_t written = fwrite(pixel_buffer, 1, totale_pixel, file);
    if (written != totale_pixel)
    {
        fprintf(stderr, "Errore: Scrittura incompleta PGM (%zu/%zu) nel file %s.\n", written, totale_pixel, nome_file);
        free(pixel_buffer);
        fclose(file);
        rilascia_tensore(img);
        exit(EXIT_FAILURE);
    }

    free(pixel_buffer);
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
    int fd = open(nome_file, O_RDONLY);
    if (fd == -1)
    {
        perror("Errore apertura file binario");
        exit(EXIT_FAILURE);
    }

    struct stat st;
    if (fstat(fd, &st) == -1)
    {
        perror("Errore fstat file binario");
        close(fd);
        exit(EXIT_FAILURE);
    }

    size_t fs = (size_t)st.st_size;
    if (fs < sizeof(struct on_disk_tensor))
    {
        fprintf(stderr, "Errore: File binario %s troppo piccolo per contenere l'intestazione.\n", nome_file);
        close(fd);
        exit(EXIT_FAILURE);
    }

    void* mapped = mmap(NULL, fs, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED)
    {
        perror("Errore mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);

    struct on_disk_tensor* hdr = (struct on_disk_tensor*)mapped;
    if (hdr->ndim <= 0 || hdr->ndim > MAX_DIM)
    {
        fprintf(stderr, "Errore: Numero di dimensioni non valido nel file binario %s.\n", nome_file);
        munmap(mapped, fs);
        exit(EXIT_FAILURE);
    }

    Tensore* t = (Tensore*)malloc(sizeof(Tensore));
    if (t == NULL)
    {
        perror("Errore: Impossibile allocare memoria per il tensore mappato");
        munmap(mapped, fs);
        exit(EXIT_FAILURE);
    }

    t->num_dim = hdr->ndim;
    size_t totale_elementi = 1;
    for (int i = 0; i < t->num_dim; i++)
    {
        if (hdr->shape[i] <= 0)
        {
            fprintf(stderr, "Errore: Forma non valida nel file binario %s.\n", nome_file);
            free(t);
            munmap(mapped, fs);
            exit(EXIT_FAILURE);
        }
        t->forma[i] = hdr->shape[i];
        totale_elementi *= (size_t)hdr->shape[i];
    }

    if (hdr->data_offset < (int64_t)sizeof(struct on_disk_tensor) || (size_t)hdr->data_offset + totale_elementi * sizeof(float) > fs)
    {
        fprintf(stderr, "Errore: Data offset non valido nel file binario %s.\n", nome_file);
        free(t);
        munmap(mapped, fs);
        exit(EXIT_FAILURE);
    }

    t->contatore_rif = 1;
    t->mmap_attivo = 1;
    t->mmap_size = fs;
    t->mmap_ptr = mapped;
    t->dati = (float*)((char*)mapped + hdr->data_offset);

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
    if (fwrite(buffer, 1, 64, f) != 64)
    {
        fprintf(stderr, "Errore: scrittura intestazione binaria fallita per %s.\n", nome_file);
        fclose(f);
        rilascia_tensore(t);
        exit(EXIT_FAILURE);
    }

    // Calcolo quanti float devo scrivere in totale
    size_t totale_elementi = 1;
    for (int i = 0; i < t -> num_dim; i++) 
    { 
        totale_elementi *= t -> forma[i]; 
    }
    
    // Scrivo tutti i dati float in un colpo solo
    if (fwrite(t -> dati, sizeof(float), totale_elementi, f) != totale_elementi)
    {
        fprintf(stderr, "Errore: scrittura dati binari incompleta per %s.\n", nome_file);
        fclose(f);
        rilascia_tensore(t);
        exit(EXIT_FAILURE);
    }

    // 5. Chiusura e pulizia
    fclose(f);
    rilascia_tensore(t);
}























