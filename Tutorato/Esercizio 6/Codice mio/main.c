#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "netpbm.h"

int main(int argc, char * argv[])
{
    /* 1. DICHIARAZIONE DELLE STRUTTURE
       Preparo le strutture per l'immagine sorgente e quella di destinazione.
    */
    netpbm image;
    netpbm target_image;

    /* 2. APERTURA SORGENTE
       Mappiamo "CM1.pgm" nella memoria virtuale. 
       In caso di errore, open_image userà perror() per spiegare il motivo.
    */
    if (open_image("CM1.pgm", &image) != 0) 
    {
        // Se l'apertura fallisce, usciamo con un codice di errore standard
        return EXIT_FAILURE;
    }

    /* 3. CREAZIONE DESTINAZIONE (Dimensioni Invertite)
       Per la trasposizione, la larghezza della nuova immagine deve essere 
       pari all'altezza della vecchia, e viceversa.
    */
    if (empty_image("CM1-transposed.pgm", &target_image, image.height, image.width) != 0) 
    {
        fprintf(stderr, "Errore fatale: Impossibile creare l'immagine trasposta.\n");
        close_image(&image); // Chiudiamo la sorgente prima di uscire
        return EXIT_FAILURE;
    }

    /* 4. ELABORAZIONE
       Avviamo la trasposizione. Grazie al memory mapping, la CPU 
       scriverà i dati direttamente sul "ponte" verso il disco.
    */
    printf("Inizio elaborazione: trasposizione di CM1.pgm (%dx%d)...\n", image.width, image.height);
    
    transpose_naive(&image, &target_image);

    /* 5. CHIUSURA E PULIZIA
       Fondamentale per sincronizzare i dati e liberare i descrittori di file.
    */
    close_image(&image);
    close_image(&target_image);
    
    printf("Trasposizione completata con successo!\n");
    printf("Il file 'CM1-transposed.pgm' è stato generato correttamente.\n");

    return EXIT_SUCCESS;
}
