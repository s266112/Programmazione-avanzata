#ifndef _NETPBM_H
#define _NETPBM_H

#include <stdio.h>

/* --- STRUTTURA E IMMAGINE PGM (Hardware-Mapped)---
   Questa struct gestisce la mappatura del file direttamente in RAM.
*/


struct _netpbm_image 
{
  int width;    // Larghezza
  int height;   // Altezza
  int offset;   // Punto in cui finisce l'header e iniziano i pixel
  int size;     // Dimensione totale del file
  FILE * fd;    // File descriptor (il canale aperto col disco)
  char * data;  // Puntatore all'inizio della memoria mappata (i nostri pixel)
};

// Definizioni di tipo 
typedef struct _netpbm_image netpbm;
typedef struct _netpbm_image * netpbm_ptr;

/* --- FUNZIONI RICHIESTE DALLA CONSEGNA --- */

// Deve aprire il file e impostare la mappatura in memoria (mmap)
int open_image(char * path, netpbm_ptr img);

// Crea un file vuoto e chiama open_image
int empty_image(char * path, netpbm_ptr img, int width, int height);

// Restituisce l'indirizzo del pixel (x, y). Fondamentale per la trasposizione!
char * pixel_at(netpbm_ptr img, int x, int y);

// Chiude la mappatura e il file
int close_image(netpbm_ptr img);

// Funzione per ruotare l'immagine usando la logica della consegna
void transpose_naive(netpbm_ptr src, netpbm_ptr dst);

#endif
