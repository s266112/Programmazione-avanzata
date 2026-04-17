/* Aggiungo questa riga per sbloccare le funzioni POSIX (fileno, ftruncate).
   Deve stare in cima a tutto, prima degli include.
*/
#define _DEFAULT_SOURCE

#include "netpbm.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>   // Header per mmap (mappatura memoria)
#include <sys/stat.h>   // Per la funzione stat (dimensione file)
#include <unistd.h>

/**
  --- OPEN_IMAGE ---:
 * Crea un collegamento diretto tra il file su disco e la RAM (mmap).
 * Restituisce 0 se tutto va bene, -1 in caso di errore.
 */
int open_image(char * path, netpbm_ptr img)
{
  // 1. APERTURA CANALE: Apertura file in lettura/scrittura ("r")
  img->fd = fopen(path, "r+");
  if (img->fd == NULL)
  return -1;

  //2. MISURAZIONE: Recupero dimensioni totali del file (per mmap)
  struct stat st;
  if (stat(path, &st) != 0)
  {
    fclose (img->fd);
    return -1;
  }
  img->size = st.st_size;

  // 3. LETTURA IDENTITA': Estraggo Width, Height e MaxVal dall'header testuale
  char magic[3];
  int max_val;
  if (fscanf(img->fd, "%2s %d %d %d%*c", magic, &img->width, &img->height, &max_val) !=4)
  {
    fclose(img->fd);
    return -1;
  }

  /* 4. IL CONFINE (CALCOLO OFFSET):
       ftell() mi dice dove finisce l'header testuale e iniziano i pixel binari.
    */
   img->offset = ftell(img->fd);

   /* 5. MAPPATURA HARDWARE (mmap):
       Collego l'intero file allo spazio di indirizzamento della CPU.
       MAP_SHARED: Assicura che le modifiche fatte in RAM vengono salvate sul disco.
    */
   int fd_raw = fileno(img->fd); // Trasforma il puntatore FILE in un numero (ID)
    img->data = mmap(NULL, img->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_raw, 0);
    
    if (img->data == MAP_FAILED) 
    {
        fclose(img->fd);
        return -1;
    }

    return 0; // Il file è "mappato" e pronto
}

/**
 * --- EMPTY_IMAGE ---
 * Crea un nuovo file PGM sul disco e lo prepara per essere riempito.
 * Una volta creato, lo aggancia alla RAM usando open_image.
 */

int empty_image(char * path, netpbm_ptr img, int width, int height)
{
  // 1. CREAZIONE FISICA: Apro il file in modalità scrittura e lettura ("w+")
  FILE * fd = fopen(path, "w+");
  if (fd == NULL) 
  {
    return -1;
  }

  /* 2. SCRITTURA HEADER: 
     Scrivo l'intestazione standard PGM (P5, dimensioni, max_val 255). 
     Il valore restituito 'written' mi dice quanti byte occupa questo testo.
  */

  int written = fprintf(fd, "P5\n%d %d\n255\n", width, height);

  /* 3. PRENOTAZIONE SPAZIO (ftruncate):
     A livello hardware, dico al sistema di "allungare" il file sul disco.
     Lo spazio totale deve essere: byte dell'header + (larghezza * altezza) byte di pixel.
     fileno(fd) serve a trasformare il puntatore C nel descrittore richiesto dal sistema.
  */
  ftruncate(fileno(fd), written + width * height);

  /* 4. PASSAGGIO DI CONSEGNE:
     Chiudo il file temporaneo. Ora che il file esiste ed è grande abbastanza,
     uso la mia open_image per mapparlo nello spazio di indirizzamento RAM.
  */
  fclose(fd);
  return open_image(path, img);
}

/**
 * --- PIXEL_AT ---
 * Trasforma le coordinate (x, y) nell'indirizzo di memoria del singolo pixel.
 */
char * pixel_at(netpbm_ptr img, int x, int y)
{
  /* LOGICA MATEMATICA E HARDWARE:
     1. img->data: Punto di inizio della memoria mappata (inizio file).
     2. img->offset: Salto i byte dell'header per non sovrascrivere i metadati.
     3. (y * img->width): Salto intere righe di pixel per scendere in verticale.
     4. (+ x): Mi sposto orizzontalmente sulla colonna corretta.
  */
  return img->data + img->offset + (y * img->width + x);
}

/**
 * --- CLOSE_IMAGE ---
 * Smonta la mappatura tra RAM e disco e chiude il file.
 * Ritorna 0 a operazione completata.
 */
int close_image(netpbm_ptr img)
{
  /* 1. MUNMAP: Lo smontaggio del ponte.
     Dico al sistema operativo che l'indirizzo img->data non deve più puntare al file sul disco.
  */
  if (img->data != MAP_FAILED)
  {
    munmap(img->data, img->size);
  }

  /* 2. FCLOSE: Chiusura del canale.
     Chiudo definitivamente il puntatore al file (fd). 
     Senza questo, il file potrebbe risultare "ancora in uso" dal sistema.
  */
  if (img->fd != NULL) 
  {
    fclose(img->fd);
  }

  return 0;
}

/**
 * --- TRANSPOSE_NAIVE ---
 Esegue la rotazione dell'immagina scambiando le coordiante X e Y.
 */
void transpose_naive(netpbm_ptr src, netpbm_ptr dst) 
{
  /* CICLO DOPPIO: Attraverso ogni riga (y) e ogni colonna (x) */
  for (int y = 0; y < src->height; y++) 
  {
    for (int x = 0; x < src->width; x++) 
    {
      /* 1. LETTURA: Valore originale */
      char pixel_value = *pixel_at(src, x, y);

      /* 2. SCRITTURA TRASPOSTA: Scambio coordinate (x,y) -> (y,x) */
      *pixel_at(dst, y, x) = pixel_value;
    }
  }
} 




