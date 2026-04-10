#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "matrix.h"

/* --- 1. GENERAZIONE MATRICE CASUALE ---
   Alloca memoria per una matrice n x n e la riempie con float 
   compresi tra 0 e 1. Utile per avere dati sempre diversi nei test.
*/
float * make_random_matrix(int n)
{
  float * matrix = (float *)malloc(sizeof(float) * n * n);
  for (int i = 0; i < n * n; i++) 
  {
    matrix[i] = (float) rand() / RAND_MAX;
  }
  return matrix;
}

/* --- 2. GENERAZIONE MATRICE VUOTA ---
   Crea una matrice n x n inizializzata interamente a zero.
   Fondamentale per i contenitori dei risultati (C1, C2, C3).
*/
float * make_zero_matrix(int n)
{
  float * matrix = (float *)malloc(sizeof(float) * n * n);
  for (int i = 0; i < n * n; i++) 
  {
    matrix[i] = 0.0;
  }
  return matrix;
}

/* --- 3. CONVERSIONE COLUMN MAJOR (TRASPOSIZIONE) ---
   Questa è la funzione chiave per le prestazioni. Prende una matrice 
   Row Major e la "ruota" in memoria.
   L'elemento in posizione (i, j) finisce in (j, i).
*/
float * to_column_major(float * M, int n)
{
  float * MT = make_zero_matrix(n);
  for (int i = 0; i < n; i++) 
  {
    for (int j = 0; j < n; j++) 
    {
      // Scambio gli indici per rendere le colonne contigue in memoria
      MT[j * n + i] = M[i * n + j];
    }
  }
  return MT;
}

/* --- 4. STAMPA A VIDEO ---
   Visualizza la matrice nel terminale. Supporta entrambi i formati
   grazie al parametro booleano 'column_major'.
*/
void print_matrix(float * M, int n, bool column_major)
{
  for (int i = 0; i < n; i++) 
  {
    for (int j = 0; j < n; j++) 
    {
      if (column_major) 
      {
        printf("%2.3f ", M[j * n + i]);
      } 
      
      else
      {
        printf("%2.3f ", M[i * n + j]);
      }
    }
    printf("\n");
  }
}
