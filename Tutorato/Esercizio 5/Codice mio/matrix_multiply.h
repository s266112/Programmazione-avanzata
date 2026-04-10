#ifndef _MATRIX_MULTIPLY_H
#define _MATRIX_MULTIPLY_H

/* --- PROTOTIPI DELLE FUNZIONI DI MOLTIPLICAZIONE --- 
   Queste dichiarazioni permettono al main.c di conoscere e chiamare 
   le funzioni implementate in matrix_multiply.c
*/

// 1. Moltiplicazione standard (Row per Row) - Il nostro punto di riferimento
void simple_multiply(float * A, float * B, float * C, int n);

// 2. Moltiplicazione ottimizzata tramite trasposizione della matrice B
void transposed_multiply(float * A, float * B, float * C, int n);

// 3. Moltiplicazione avanzata a blocchi (Tiling) per massimizzare la Cache
void blocked_multiply(float * A, float * B, float * C, int n);

#endif
