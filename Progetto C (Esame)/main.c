// Alessio Matessi  S266112

#include <stdio.h>
#include "tensorforth.h"

int main()
{
    printf("--- COLLAUDO FILL (f) E RIDUZIONE (S) ---\n\n");

    Stack* s = crea_stack();

    // =========================================================
    // TEST FILL (f)
    // =========================================================
    // 1. Forma richiesta: [2 3] (Matrice 2x3)
    int32_t f_shape[1] = {2};
    Tensore* s_shape = crea_tensore(1, f_shape);
    s_shape->dati[0] = 2.0f; s_shape->dati[1] = 3.0f;
    push(s, s_shape);

    // 2. Valori da ripetere: [1.0 2.0]
    int32_t f_v[1] = {2};
    Tensore* v = crea_tensore(1, f_v);
    v->dati[0] = 1.0f; v->dati[1] = 2.0f;
    push(s, v);

    printf("Esecuzione Fill (f)...\n");
    op_fill(s); 

    // Duplico temporaneamente il puntatore della cima per stampare senza fare pop
    Tensore* res_fill = s->cima->tensore; 
    printf("Risultato Fill (Attesa matrice 2x3 con 1.0 e 2.0 ripetuti):\n");
    stampa_tensore(res_fill);

    // =========================================================
    // TEST RIDUZIONE (S)
    // =========================================================
    printf("Esecuzione Riduzione (S) sulla matrice appena generata...\n");
    op_somma_riduzione(s);

    Tensore* res_riduzione = pop(s);
    printf("Risultato Riduzione (Somma totale attesa: 9.0):\n");
    stampa_tensore(res_riduzione);

    rilascia_tensore(res_riduzione);
    libera_stack(s);

    printf("--- COLLAUDO TERMINATO ---\n");

    return 0;
}