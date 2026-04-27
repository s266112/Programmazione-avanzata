#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <omp.h>

#include "netpbm.h"

/**
 * --- OPEN_IMAGE ---
 * Proietta il file PGM dal disco alla memoria virtuale.
 */
int open_image(char * path, netpbm_ptr img) 
{
    /* 1. APERTURA FILE
       Si apre il file in modalità r+ per permettere la mappatura in scrittura.
    */
    img->fd = fopen(path, "r+");
    if (img->fd == NULL) 
    {
        return -1;
    }

    /* 2. MISURAZIONE HARDWARE
       Recupero della dimensione totale del file binario tramite stat.
    */
    struct stat sbuf;
    stat(path, &sbuf);
    img->size = sbuf.st_size;

    /* 3. PARSING HEADER
       Lettura delle dimensioni dell'immagine dall'intestazione testuale.
    */
    if (fscanf(img->fd, "P5\n%d %d\n255\n", &img->width, &img->height) != 2) 
    {
        fclose(img->fd);
        return -2;
    }

    /* 4. MAPPATURA MEMORIA
       Calcolo dell'offset e attivazione del memory mapping (mmap).
    */
    img->offset = ftell(img->fd);
    img->data = mmap((void *)0, img->size, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(img->fd), 0);
    
    if (img->data == MAP_FAILED) 
    {
        fclose(img->fd);
        return -3;
    }
    return 0;
}

/**
 * --- PIXEL_AT ---
 * Restituisce il puntatore al pixel in coordinate (x,y).
 */
char *pixel_at(netpbm_ptr img, int x, int y) 
{
    /* 1. VERIFICA CONFINI
       Controllo dei limiti per prevenire accessi illegali in memoria.
    */
    if (img == NULL) 
    {
        return NULL;
    }
    if (x < 0 || x >= img->width) 
    {
        return NULL;
    }
    if (y < 0 || y >= img->height) 
    {
        return NULL;
    }

    /* 2. ACCESSO DIRETTO
       Traduzione dell'indice 2D in indirizzo fisico lineare.
    */
    return &img->data[y * img->width + x + img->offset];
}

/**
 * --- CLOSE_IMAGE ---
 * Rilascia le risorse hardware impegnate.
 */
int close_image(netpbm_ptr img) 
{
    if (img == NULL) 
    {
        return -1;
    }

    /* 1. PULIZIA
       Smontaggio della mappatura mmap e chiusura del descrittore.
    */
    munmap(img->data, img->size);
    fclose(img->fd);
    return 0;
}

/**
 * --- CREATE_OUTPUT_IMAGE ---
 * Crea il file di destinazione e ne riserva lo spazio fisico sul disco.
 */
int create_output_image(char *path, netpbm_ptr src, netpbm_ptr dst, int pixel_mask_size) 
{
    /* 1. VALIDAZIONE */
    if (path == NULL || src == NULL || dst == NULL) 
    {
        return -1;
    }

    /* 2. CALCOLO NUOVE DIMENSIONI
       Riduzione dell'area di output in base al raggio del filtro scelto.
    */
    int dst_width  = src->width  - 2 * pixel_mask_size;
    int dst_height = src->height - 2 * pixel_mask_size;

    if (dst_width <= 0 || dst_height <= 0) 
    {
        return -2;
    }

    /* 3. SETUP FILE E FTRUNCATE
       Viene scritto l'header e usata ftruncate per allocare i blocchi sul disco.
    */
    FILE *fd = fopen(path, "w+");
    if (fd == NULL) 
    {
        return -3;
    }

    int written = fprintf(fd, "P5\n%d %d\n255\n", dst_width, dst_height);
    if (written < 0) 
    {
        fclose(fd);
        return -4;
    }

    if (ftruncate(fileno(fd), written + dst_width * dst_height) != 0) 
    {
        fclose(fd);
        return -5;
    }
    fclose(fd);

    /* 4. RIAPERTURA IN MODALITÀ MAPPED */
    return open_image(path, dst);
}