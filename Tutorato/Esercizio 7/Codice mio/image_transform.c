#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <omp.h>

#include "netpbm.h"
#include "image_transform.h"


/*
 --- BUILD_PREFIX_SUM ---
- Costruisce la tabella P delle somme cumulative.
- Permette di ottenere la somma dei pixel di qualsiasi area rettangolare in O(1).
*/

unsigned long long *build_prefix_sum(const unsigned char *src_data, int w, int h)
{
    /* 1. ALLOCAZIONE E INIZIALIZZAZIONE
       Si alloca una matrice di dimensioni (w+1) * (h+1).
       L'uso di calloc è fondamentale per azzerare la riga 0 e la colonna 0, che fungono da base neutra per il calcolo ricorsivo ai bordi.
    */

    size_t size = (size_t)(h + 1) * (w + 1);
    unsigned long long *prefix = calloc(size, sizeof(unsigned long long));

    if (prefix == NULL) 
    {
        return NULL;
    }

    /* 2. CALCOLO DELLA MATRICE P (DYNAMIC PROGRAMMING)
       L'algoritmo scorre l'immagine riga per riga. Ogni cella P(x,y) accumula il valore del pixel corrente 
       più le somme già calcolate a sinistra e sopra. Viene sottratto il valore in alto a sinistra per evitare il doppio conteggio.
    */
   for (int y = 1; y<= h; y++)
    {
        for (int x = 1; x <=w; x++)
        
        {
            // I pixel dell'immagine originale partono da indice 0 (y-1, x-1)
            unsigned char pixel_val =src_data[(y -1) * w + (x - 1)];
            
            // Formula: P(x,y) = M(x,y) + P(x-1,y) + P(x,y-1) - P(x-1,y-1)
            prefix[y * (w + 1) + x] = (unsigned long long)pixel_val 
                                    + prefix[(y - 1) * (w + 1) + x] 
                                    + prefix[y * (w + 1) + (x - 1)] 
                                    - prefix[(y - 1) * (w + 1) + (x - 1)];
        }
    }
    return prefix;
}

/*
    --- BLUR_IMAGE ---
 - Applica un filtro di sfocatura (box blur) all'immagine.
 - Utilizza OpenMP per parallelizzare il calcolo sulle righe.
 */

int blur_image(netpbm_ptr src, char *path, netpbm_ptr dst) 
{
    /* 1. CONTROLLO PRELIMINARE 
       Verifica della validità dei puntatori passati come argomento,
       come previsto nel template originale del professore.
    */
    
    if (src == NULL || dst == NULL || path == NULL) 
    {
        return -1;
    }

    /* 2. PREPARAZIONE OUTPUT E TABELLA P
       Creo il file di destinazione. La funzione create_output_image riduce le dimensioni dell'immagine 
       per evitare di gestire i bordi fuori limite. Successivamente costruisco la tabella delle somme cumulative.
    */
    if (create_output_image(path, src, dst, RADIUS_BLUR) != 0) 
    {
        return -2; // Errore nella creazione del file
    }

    unsigned long long *P = build_prefix_sum((unsigned char *)src->data + src->offset, src->width, src->height);
    if (P == NULL) 
    {
        return -3; // Errore allocazione memoria P
    }

    int w = src->width;
    int r = RADIUS_BLUR;
    int area = (2 * r + 1) * (2 * r + 1);

    /* 3. CALCOLO PARALLELO (OpenMP)
       Distribuzione del carico sulle righe. La formula D - C - B + A 
       permette il calcolo della media locale in tempo costante.
    */
    #pragma omp parallel for schedule(static)
    for (int y = 0; y < dst->height; y++) 
    {
        for (int x = 0; x < dst->width; x++) 
        {
            
            // Coordinate centrate nell'immagine originale (traslate del raggio + offset tabella)
            int src_x = x + r + 1;
            int src_y = y + r + 1;

            /* 4. RECUPERO VERTICI 
            Formula: A = P(x-r, y-r), B = P(x+r, y-r), C = P(x-r, y+r), D = P(x+r, y+r)
            */
            unsigned long long A = P[(src_y - r - 1) * (w + 1) + (src_x - r - 1)];
            unsigned long long B = P[(src_y - r - 1) * (w + 1) + (src_x + r)];
            unsigned long long C = P[(src_y + r) * (w + 1) + (src_x - r - 1)];
            unsigned long long D = P[(src_y + r) * (w + 1) + (src_x + r)];

            unsigned long long sum = D - C - B + A;

            // Salvataggio del pixel mediato
            dst->data[dst->offset + (y * dst->width + x)] = (unsigned char)(sum / area);
        }
    }

    /* 5. PULIZIA
       Liberazione della memoria heap utilizzata per la tabella P.
    */
    free(P);
    return 0;
}

/*
 --- EDGE_HIGHLIGHT ---
 - Identifica ed evidenzia i bordi dell'immagine.
 - Un pixel diventa bianco se la sua intensità è significativamente superiore alla media dell'area circostante.
 */

int edge_highlight(netpbm_ptr src, char *path, netpbm_ptr dst) 
{

    /* 1. CONTROLLO PRELIMINARE E SETUP
       Verifica dei puntatori e inizializzazione dell'immagine di output con il raggio specifico.
    */

    if (src == NULL || dst == NULL || path == NULL) 
    {
        return -1;
    }

    if (create_output_image(path, src, dst, RADIUS_BORDER_HIGHLIGHT) != 0)
    {
        return -2;
    }

    /* 2. COSTRUZIONE TABELLA P
       Generazione della Prefix Sum per il calcolo rapido della media locale.
    */
    unsigned long long *P = build_prefix_sum((unsigned char *)src->data + src->offset, src->width, src->height);
    if (P == NULL) 
    {
        return -3;
    }

    int w = src->width;
    int r = RADIUS_BORDER_HIGHLIGHT;
    int area = (2 * r + 1) * (2 * r + 1);

    /* 3. ELABORAZIONE PARALLELA (OpenMP)
       Per ogni pixel, calcolo la media dell'area circostante.
       Se il pixel originale è > della media + soglia (valore empirico 10), lo marchio come bordo.
    */
    #pragma omp parallel for schedule(static)
    for (int y = 0; y < dst->height; y++) 
    {
        for (int x = 0; x < dst->width; x++) 
        {
            
            // Coordinate centrate nell'immagine originale
            int src_x = x + r + 1;
            int src_y = y + r + 1;

            // Recupero vertici per la somma dell'area (Formula PDF)
            unsigned long long A = P[(src_y - r - 1) * (w + 1) + (src_x - r - 1)];
            unsigned long long B = P[(src_y - r - 1) * (w + 1) + (src_x + r)];
            unsigned long long C = P[(src_y + r) * (w + 1) + (src_x - r - 1)];
            unsigned long long D = P[(src_y + r) * (w + 1) + (src_x + r)];

            unsigned long long sum = D - C - B + A;
            unsigned char local_avg = (unsigned char)(sum / area);
            
            // Valore del pixel centrale originale
            unsigned char central_pixel = (unsigned char)src->data[src->offset + (src_y - 1) * w + (src_x - 1)];

            /* 4. LOGICA DI SOGLIA
               Se la differenza tra il pixel e la media è marcata, è un bordo.
               Uso 255 (Bianco) per il bordo e 0 (Nero) per il resto.
            */
           if (central_pixel > local_avg + 10)

           {
            // Uso il cast a (unsigned char) per evitare il warning di overflow
            dst->data[dst->offset + (y * dst->width + x)] = (unsigned char)255;
           }
           else
           {
            dst->data[dst->offset + (y * dst->width + x)] = 0;
           }

        }
    }

    /* 5. PULIZIA
       Liberazione della tabella P al termine del processo.
    */
    free(P);
    return 0;
}
