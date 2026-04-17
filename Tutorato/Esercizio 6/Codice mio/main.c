#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "netpbm.h"

int main(int argc, char * argv[])
{
  /* 1. DICHIARAZIONE DELLE STRUTTURE
     Preparo le etichette per l'immagine sorgente (image) e quella di destinazione (target_image).
  */
  netpbm image;
  netpbm target_image;

  /* 2. APERTURA SORGENTE
     Chiamo open_image per mappare "CM1.pgm" nella RAM.
     Se il file non esiste nella cartella, err sarà != 0.
  */
  int err = open_image("CM1.pgm", &image);
  if (err != 0) 
  {
    printf("Unable to open source image: %d\n", -err);
    return 0;
  }

  /* 3. CREAZIONE DESTINAZIONE
     Creo il file per la rotazione
  */
  err = empty_image("CM1-transposed.pgm", &target_image, image.height, image.width);
  if (err != 0) 
  {
    printf("Unable to open target image: %d\n", -err);
    return 0;
  }

  /* 4. ELABORAZIONE (Loop dei Pixel)
     Eseguo la rotazione vera e propria
  */
  transpose_naive(&image, &target_image);

  /* 5. CHIUSURA E PULIZIA
     Smonto le mappature per salvare i dati sul disco.
  */
  close_image(&image);
  close_image(&target_image);
  
  printf("Trasposizione completata con successo!\n");
  return 0;
}
