#include "matrix_multiply.h"

/*  --- 1. MOLTIPLICAZIONE STANDARD ---
    Questa è l'implementazione classica "riga per colonna".
    Viene usata come riferimento per verificare la correttezza delle altre.
*/

void simple_multiply(float * A, float * B, float * C, int n)
{
  // Ciclo i: Scorre le righe della matrice A
  for (int i = 0; i < n; i++) 
  {
    // Ciclo j: Scorre tutte le colonne della matrice B
    for (int j = 0; j < n; j++) 
    {
      // Ciclo k: Effettua il prodotto scalare tra la riga i e la colonna j
      for (int k = 0; k < n; k++) 
      {
        /* PROBLEMA DI PERFORMANCE:
           Mentre A[i * n + k] legge dati contigui in memoria (Row Major),
           B[k * n + j] salta di riga in riga per restare sulla stessa colonna. 
           Questi salti causano "Cache Miss", costringendo la CPU ad aspettare la RAM.
        */
	    C[i * n + j] += A[i * n + k] * B[k * n + j];
      }
    }
  }
}


/* --- 2. MOLTIPLICAZIONE CON TRASPOSIZIONE ---
   In questa variante, sfruttiamo il fatto che la matrice B sia stata "girata" 
   (Column Major). Questo rende l'accesso ai dati molto più fluido per la cache.
*/
void transposed_multiply(float * A, float * B, float * C, int n)
{
  // Scorro le righe della matrice Risultato C
  for (int i = 0; i < n; i++)
  {
    // Scorro le colonne della matrice Risultato C
    for (int j = 0; j < n; j++)
    {
      float sum = 0.0;

      /* CALCOLO SEQUENZIALE:
         Scorro contemporaneamente la riga di A e la colonna di B.
         Grazie alla trasposizione, B[j * n + k] legge elementi vicini in memoria.
      */
     for (int k = 0; k < n; k++)
     {
      sum += A[i * n + k] * B[j * n + k];
     }

     // Salvo il totale nella cella corretta C
     C[i * n + j] = sum;
    }
  }
}


/* --- 3. KERNEL (CALCOLO A BLOCCHI) ---
   Questa funzione moltiplica solo un "sottoblocco" delle matrici.
   Lavora su dati piccoli per minimizzare i salti in RAM.
*/
void kernel(float * A, float * B, float * C, int x, int dx, int y, int dy, int z, int dz, int n)
{
  // Ciclo i: righe del blocco (limite n per sicurezza)
  for (int i = x; i < x + dx && i < n; i++)
  {
    // Ciclo j: colonne del blocco (limite n per sicurezza)
    for (int j = y; j < y + dy && j < n; j++)
    {
      // Ciclo k: scorrimento interno per il prodotto scalare
      for (int k = z; k < z + dz && k < n; k++)
      {
        /* REGOLA
           Uso l'accesso trasposed B[j * n + k] perchè anche 
           nei blocchi voglio la massima velocità ! 
        */
        C[i * n + j] += A[i * n + k] * B[j*n + k];
      }
    } 
  } 
}


/* --- 4. MOLTIPLICAZIONE A BLOCCHI (TILING) ---
   Divide le matrici in blocchi di dimensione fissa (s1, s2, s3)
   e chiama il kernel per processarli uno ad uno.
*/
void blocked_multiply(float * A, float * B, float * C, int n)
{
  // Dimensioni del blocco (16 è un valore bilanciato per una cache)
  const int s1 = 16; 
  const int s2 = 16;
  const int s3 = 16;

  /* CICLI ESTERNI:
     Saltano di 'step' in 'step' (i += s1) per identificare
     il punto di inizio di ogni blocco nelle matrici.
  */
 for (int i = 0; i < n; i += s1)
 {
  for (int j = 0; j < n; j += s2)
  {
    for (int k = 0; k < n; k += s3)
    {
      // Chiamo il kernel passandogli le coordinate del blocco attuale
      kernel (A, B, C, i, s1, j, s2, k, s3, n);
    }
  }
 }
}
