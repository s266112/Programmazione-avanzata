#ifndef _MATRIX_H
#define _MATRIX_H

#include <stdbool.h>

/* --- INTERFACCIA PER LA GESTIONE MATRICI --- */

// Crea una matrice con valori casuali
float * make_random_matrix(int n);

// Crea una matrice inizializzata a zero
float * make_zero_matrix(int n);

// Trasforma una matrice in formato Column Major
float * to_column_major(float * M, int n);

// Stampa la matrice a video gestendo i diversi formati di memoria
void print_matrix(float * M, int n, bool column_major);

#endif
