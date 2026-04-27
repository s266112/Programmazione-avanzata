#ifndef IMAGE_TRANSFORM_H
#define IMAGE_TRANSFORM_H

/* 1. COSTANTI DI ELABORAZIONE
   Definiscono il raggio d'azione dei filtri. 
   Un raggio maggiore aumenta l'area di campionamento per ogni pixel.
*/
#define RADIUS_BLUR 50
#define RADIUS_BORDER_HIGHLIGHT 250

/**
 * --- PROTOTIPI DELLE FUNZIONI DI TRASFORMAZIONE ---
 * Queste funzioni implementano la logica di calcolo parallelo (OpenMP)
 * sfruttando la tabella delle somme cumulative.
 */

/* 1. Applica l'effetto sfocatura (Box Blur) all'immagine sorgente */
int blur_image(netpbm_ptr src, char *path, netpbm_ptr dst);

/* 2. Genera la tabella P (Prefix Sum) necessaria per l'ottimizzazione O(1) */
unsigned long long *build_prefix_sum(const unsigned char *src_data, int w, int h);

/* 3. Evidenzia i bordi basandosi sulla differenza di intensità locale */
int edge_highlight(netpbm_ptr src, char *path, netpbm_ptr dst);

/* 4. Rilascia la memoria dell'immagine trasformata */
int close_image(netpbm_ptr img);

#endif