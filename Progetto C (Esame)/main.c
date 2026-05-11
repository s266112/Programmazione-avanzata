// Alessio Matessi  S266112

#include <stdio.h>
#include "tensorforth.h"

int main()
{
    printf("--- COLLAUDO PRODOTTO DI MATRICI (@) ---\n\n");

    Stack* s = crea_stack();

    // =========================================================
    // CREAZIONE MATRICI
    // =========================================================
    
    // Matrice B (sotto): 3 righe x 2 colonne
    int32_t forma_b[2] = {3, 2};
    Tensore* b_mat = crea_tensore(2, forma_b);
    b_mat->dati[0] = 7.0;  b_mat->dati[1] = 8.0;
    b_mat->dati[2] = 9.0;  b_mat->dati[3] = 10.0;
    b_mat->dati[4] = 11.0; b_mat->dati[5] = 12.0;

    // Matrice A (cima): 2 righe x 3 colonne
    int32_t forma_a[2] = {2, 3};
    Tensore* a_mat = crea_tensore(2, forma_a);
    a_mat->dati[0] = 1.0; a_mat->dati[1] = 2.0; a_mat->dati[2] = 3.0;
    a_mat->dati[3] = 4.0; a_mat->dati[4] = 5.0; a_mat->dati[5] = 6.0;

    printf("Matrice B (sotto, 3x2):\n");
    stampa_tensore(b_mat);
    
    printf("\nMatrice A (cima, 2x3):\n");
    stampa_tensore(a_mat);

    // =========================================================
    // ESECUZIONE
    // =========================================================
    
    // Inserimento nello stack (prima B, poi A in cima)
    push(s, b_mat);
    push(s, a_mat);

    printf("\nCalcolo in corso (A @ B)...\n\n");
    op_prodotto_matrici(s);

    // =========================================================
    // ESTRAZIONE E PULIZIA
    // =========================================================
    
    Tensore* ris = pop(s);
    printf("Risultato (deve essere 2x2):\n");
    stampa_tensore(ris);
    rilascia_tensore(ris);

    libera_stack(s);
    printf("\n--- COLLAUDO TERMINATO ---\n");

    return 0;
}