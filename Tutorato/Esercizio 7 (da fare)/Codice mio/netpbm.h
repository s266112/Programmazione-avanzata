#ifndef NETPBM_H
#define NETPBM_H

#include <stdio.h>

/**
 * --- STRUTTURA NETPBM ---
 * Gestisce i metadati e il puntatore alla memoria mappata dell'immagine.
 */
typedef struct _netpbm_image 
{
    /* 1. DIMENSIONI 
       Larghezza e altezza dell'immagine in pixel.
    */
    int width;
    int height;

    /* 2. GESTIONE MEMORIA HARDWARE
       offset: Indica dove finisce l'header testuale e iniziano i dati binari.
       size: La dimensione totale del file su disco (in byte).
    */
    int offset;
    int size;

    /* 3. PUNTATORI E DESCRITTORI
       fd: Puntatore al file aperto (File Descriptor).
       data: Puntatore all'inizio della memoria mappata (il "ponte" verso il disco).
    */
    FILE *fd;
    char *data;
} netpbm;

/* Definizione del tipo puntatore per semplificare la sintassi nel progetto */
typedef netpbm *netpbm_ptr;

/**
 * --- PROTOTIPI DELLE FUNZIONI ---
 * Rispettano rigorosamente la firma fornita dal professore.
 */

/* 1. Carica l'immagine creando il mapping tra RAM e Disco */
int open_image(char *path, netpbm_ptr img);

/* 2. Navigazione: restituisce l'indirizzo di memoria del pixel (x,y) */
char *pixel_at(netpbm_ptr img, int x, int y);

/* 3. Creazione: prepara un nuovo file PGM dimensionato per la trasformazione */
int create_output_image(char *path, netpbm_ptr src, netpbm_ptr dst, int pixel_mask_size);

#endif