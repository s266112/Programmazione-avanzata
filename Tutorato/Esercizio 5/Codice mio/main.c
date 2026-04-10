#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "matrix.h"
#include "matrix_multiply.h"

// Definizioni non utilizzate nel test principale, mantenute per compatibilità
#define NN 10
#define MM 10
#define PP 10

/* --- 1. FUNZIONE DI BENCHMARK (TEST PRESTAZIONI) ---
   Questa funzione automatizza la misurazione del tempo. 
   Ho aggiunto il parametro 'use_transpose' perché le mie funzioni 
   ottimizzate richiedono che la matrice B sia in formato Column Major. 
*/

float test_multiply(void (* multiply)(float *, float *, float *, int), bool use_transponse)
{
  int n = 1000; // Test su larga scala (1000x1000) per ottimizzare la cache
  float * A = make_random_matrix(n);
  float * B = make_random_matrix(n);
  float * C = make_zero_matrix(n);


  /* LOGICA DI PREPARAZIONE: Se la funzione da testare è ottimizzata, devo "girare" B in memoria prima di far partire il cronometro. */
  float * mat_to_use = B;
  if (use_transponse)
  {
    mat_to_use = to_column_major(B, n);
  }

  // Inizio misurazione
  clock_t start = clock();
  multiply(A, mat_to_use, C, n);
  clock_t end = clock();

  // Calcolo del tempo in millisecondi
  float ms = (float) (end - start) / (CLOCKS_PER_SEC / 1000.0);


 /* PULIZIA: Libero tutta la memoria per evitare "memory leak" durante i test ripetuti. */

  if (use_transponse) free (mat_to_use);

  free(A);
  free(B);
  free(C);

  return ms;
}


/* --- 2. FUNZIONE PRINCIPALE (ESECUZIONE) --- */
int main (int argc, char * argv[])
{
  const int n = 8;  // Dimensione ridotta per la verifica visiva dei calcoli


  // Alloco le matrici di base
  float * A = make_random_matrix(n);
  float * B = make_random_matrix(n);


  // Creo la versione Column Major (Trasposta) per i test corretti
  float * BT = to_column_major(B, n);


  // Contenitori per i risultati dei tre metodi
  float * C1 = make_zero_matrix(n);
  float * C2 = make_zero_matrix(n);
  float * C3 = make_zero_matrix(n);


  /* --- VERIFICA DI CORRETTEZZA --- 
     Confronto i risultati. Devono essere identici per confermare 
     che le ottimizzazioni non hanno alterato il calcolo matematico. 
  */

  printf("Test di Correttezza (Matrici %dx%d):\n", n, n);

  // Metodo standard (Baseline)
  simple_multiply(A, B, C1, n);
  printf("Risultato Standard (C1) stampato per riferimento.\n");
  print_matrix(C1, n, false);
  printf("\n");

  // Metodo Trasposto
  printf("Risultato Trasposta (C2) - Deve corrispondere a C1:\n");
  transposed_multiply(A, BT, C2, n); 
  print_matrix(C2, n, false);
  printf("\n");

  // Metodo a Blocchi (Tiling)
  printf("Risultato a Blocchi (C3) - Deve corrispondere a C1:\n");
  blocked_multiply(A, BT, C3, n);    
  print_matrix(C3, n, false);
  printf("\n");


  /* --- ANALISI PRESTAZIONALE --- 
     Misuro l'efficienza reale su matrici grandi (1000x1000).
  */
  printf("Analisi Performance (Benchmark):\n");


  // Nota: passiamo 'true' solo per le funzioni che leggono la matrice trasposta
  printf("- Standard Multiplication:   %f ms\n", test_multiply(simple_multiply, false));
  printf("- Transposed Multiplication: %f ms\n", test_multiply(transposed_multiply, true));
  printf("- Blocked Multiplication:    %f ms\n", test_multiply(blocked_multiply, true));

  // LIBERAZIONE MEMORIA FINALE
  free(A); free(B); free(BT);
  free(C1); free(C2); free(C3);

  return 0;
}
