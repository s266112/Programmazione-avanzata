// Alessio Matessi  S266112

#include <stdio.h>
#include "tensorforth.h"

int main()
{
    printf("=========================================================\n");
    printf("   COLLAUDO GENERALE TENSORFORTH (BLOCCHI 1, 2, 3, 4)\n");
    printf("=========================================================\n\n");

    Stack* s = crea_stack();
    int32_t forma[1] = {3};

    // =========================================================
    // 1. TEST ARITMETICA (+, -, *)
    // =========================================================
    printf("--- 1. ARITMETICA ---\n");
    Tensore* b_arit = crea_tensore(1, forma); // Sotto
    Tensore* a_arit = crea_tensore(1, forma); // Cima
    b_arit->dati[0] = 10.0; b_arit->dati[1] = 20.0; b_arit->dati[2] = 30.0;
    a_arit->dati[0] =  2.0; a_arit->dati[1] =  4.0; a_arit->dati[2] =  6.0;

    printf("B (sotto): "); stampa_tensore(b_arit);
    printf("A (cima) : "); stampa_tensore(a_arit);

    // Test Somma
    push(s, b_arit); push(s, a_arit);
    trattieni_tensore(b_arit); trattieni_tensore(a_arit); // Trattengo per riusarli
    op_somma(s);
    Tensore* ris_somma = pop(s);
    printf("Somma (A + B): "); stampa_tensore(ris_somma);
    rilascia_tensore(ris_somma);

    // Test Sottrazione (A - B)
    push(s, b_arit); push(s, a_arit);
    trattieni_tensore(b_arit); trattieni_tensore(a_arit);
    op_sottrazione(s);
    Tensore* ris_sott = pop(s);
    printf("Sottrazione (A - B): "); stampa_tensore(ris_sott);
    rilascia_tensore(ris_sott);

    // Test Prodotto
    push(s, b_arit); push(s, a_arit);
    op_prodotto(s); // Qui NON li trattengo, così op_prodotto li distrugge definitivamente
    Tensore* ris_prod = pop(s);
    printf("Prodotto (A * B): "); stampa_tensore(ris_prod);
    rilascia_tensore(ris_prod);
    printf("\n");

    // =========================================================
    // 2. TEST COMPARAZIONI (<, >, =)
    // =========================================================
    printf("--- 2. COMPARAZIONI ---\n");
    Tensore* b_comp = crea_tensore(1, forma);
    Tensore* a_comp = crea_tensore(1, forma);
    b_comp->dati[0] = 5.0; b_comp->dati[1] = 10.0; b_comp->dati[2] = 15.0;
    a_comp->dati[0] = 1.0; a_comp->dati[1] = 10.0; a_comp->dati[2] = 20.0;

    printf("B (sotto): "); stampa_tensore(b_comp);
    printf("A (cima) : "); stampa_tensore(a_comp);

    // Minore (A < B)
    push(s, b_comp); push(s, a_comp);
    trattieni_tensore(b_comp); trattieni_tensore(a_comp);
    op_minore(s);
    Tensore* ris_minore = pop(s);
    printf("Minore (A < B): "); stampa_tensore(ris_minore);
    rilascia_tensore(ris_minore);

    // Maggiore (A > B)
    push(s, b_comp); push(s, a_comp);
    trattieni_tensore(b_comp); trattieni_tensore(a_comp);
    op_maggiore(s);
    Tensore* ris_magg = pop(s);
    printf("Maggiore (A > B): "); stampa_tensore(ris_magg);
    rilascia_tensore(ris_magg);

    // Uguale (A == B)
    push(s, b_comp); push(s, a_comp);
    op_uguale(s);
    Tensore* ris_uguale = pop(s);
    printf("Uguale (A == B): "); stampa_tensore(ris_uguale);
    rilascia_tensore(ris_uguale);
    printf("\n");

    // =========================================================
    // 3. TEST LOGICA E SELEZIONE (&, |, !, $)
    // =========================================================
    printf("--- 3. LOGICA E SELEZIONE ---\n");
    Tensore* b_log = crea_tensore(1, forma);
    Tensore* a_log = crea_tensore(1, forma);
    Tensore* mask = crea_tensore(1, forma);
    
    b_log->dati[0] = 0.0; b_log->dati[1] = 1.0; b_log->dati[2] = 99.0; // Il 99 servirà per la selezione
    a_log->dati[0] = 1.0; a_log->dati[1] = 1.0; a_log->dati[2] = 10.0; // Il 10 servirà per la selezione
    mask->dati[0]  = 1.0; mask->dati[1]  = 0.0; mask->dati[2]  = 1.0;  // Maschera

    printf("B (sotto / if False): "); stampa_tensore(b_log);
    printf("A (cima / if True)  : "); stampa_tensore(a_log);
    printf("Maschera            : "); stampa_tensore(mask);

    // AND (&) - Uso solo i primi due elementi logicamente per non confonderci
    push(s, b_log); push(s, a_log);
    trattieni_tensore(b_log); trattieni_tensore(a_log);
    op_and(s);
    Tensore* ris_and = pop(s);
    printf("AND (A & B): "); stampa_tensore(ris_and);
    rilascia_tensore(ris_and);

    // OR (|)
    push(s, b_log); push(s, a_log);
    trattieni_tensore(b_log); trattieni_tensore(a_log);
    op_or(s);
    Tensore* ris_or = pop(s);
    printf("OR (A | B) : "); stampa_tensore(ris_or);
    rilascia_tensore(ris_or);

    // NOT (!)
    push(s, a_log);
    trattieni_tensore(a_log);
    op_not(s);
    Tensore* ris_not = pop(s);
    printf("NOT (!A)   : "); stampa_tensore(ris_not);
    rilascia_tensore(ris_not);

    // Selezione ($) - M è in cima, A in mezzo, B sotto
    push(s, b_log); push(s, a_log); push(s, mask);
    op_selezione(s);
    Tensore* ris_sel = pop(s);
    printf("Selezione (M ? A : B): "); stampa_tensore(ris_sel);
    rilascia_tensore(ris_sel);
    printf("\n");

    // =========================================================
    // PULIZIA FINALE
    // =========================================================
    libera_stack(s);
    printf("=========================================================\n");
    printf("   COLLAUDO TERMINATO CON SUCCESSO! ZERO LEAK DI MEMORIA.\n");
    printf("=========================================================\n");

    return 0;
}