// Alessio Matessi  S266112

#include <stdio.h>
#include "tensorforth.h"

int main()
{
    printf("--- INIZIO COLLAUDO STACK E MOTORE ---\n\n");

    // 1. Inizializziamo lo Stack
    printf("1. Creo uno stack vuoto...\n");
    Stack* mio_stack = crea_stack();
    printf("   Fatto!\n\n");

    // 2. Creiamo un tensore
    int32_t forma_prova[2] = {3, 3};
    printf("2. Creo un nuovo tensore 3x3...\n");
    Tensore* mio_tensore = crea_tensore(2, forma_prova);
    printf("   Tensore creato. Contatore_rif: %d\n\n", mio_tensore->contatore_rif);

    // 3. Facciamo un Push
    printf("3. Inserisco il tensore nello stack (PUSH)...\n");
    push(mio_stack, mio_tensore);
    printf("   Fatto! Ora c'e' un elemento in cima.\n\n");

    // 4. Facciamo un Pop
    printf("4. Estraggo il tensore dallo stack (POP)...\n");
    Tensore* tensore_estratto = pop(mio_stack);
    
    // Controlliamo che l'estrazione abbia restituito esattamente il nostro tensore
    if (tensore_estratto == mio_tensore) {
        printf("   Successo! Ho recuperato lo stesso identico tensore.\n\n");
    }

    // 5. Pulizia finale
    printf("5. Distruggo il tensore e libero lo stack...\n");
    rilascia_tensore(mio_tensore); // Distruggiamo il tensore che avevamo in mano
    libera_stack(mio_stack);       // Distruggiamo lo stack vuoto
    printf("   Fatto! Memoria ripulita senza errori.\n\n");

    printf("--- COLLAUDO TERMINATO CON SUCCESSO ---\n");

    return 0;
}