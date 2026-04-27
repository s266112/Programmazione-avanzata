#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "netpbm.h"
#include "image_transform.h"

#ifndef FILENAMES
#define REFERENCE "kitty.pgm"
#define BLURRED "kitty-transformed-blurred.pgm"
#define EDGE "kitty-transformed-edgeselection.pgm"
#endif

#ifndef SEP
#define SEP "\t"
// #define SEP ","
#endif

/**
 * --- BENCHMARK_IMAGES ---
 * Esegue i filtri più volte per calcolare una media precisa dei tempi.
 */
int benchmark_images(netpbm *image, int num_threads, int n_iter) 
{
    netpbm blurred, edge;
    double start, end;
    double blur_time = 0.0;
    double edge_time = 0.0;
    int err;

    /* 1. SETUP THREAD
       Impostto il numero di core per questa specifica esecuzione.
    */
    omp_set_num_threads(num_threads);

    for (int i = 0; i < n_iter; i++) 
    {
        /* 2. MISURAZIONE BLUR
           Calcolo del tempo di esecuzione del filtro sfocatura.
        */
        start = omp_get_wtime();
        err = blur_image(image, BLURRED, &blurred);
        end = omp_get_wtime();

        if (err != 0) 
        {
            fprintf(stderr, "Unable to blur image: %d\n", -err);
            return err;
        }
        blur_time += end - start;

        /* 3. MISURAZIONE EDGE
           Calcolo del tempo per l'evidenziazione dei bordi.
        */
        start = omp_get_wtime();
        err = edge_highlight(image, EDGE, &edge);
        end = omp_get_wtime();

        if (err != 0) 
        {
            fprintf(stderr, "Unable to compute edge_highlight: %d\n", -err);
            close_image(&blurred);
            return err;
        }
        edge_time += end - start;

        /* 4. PULIZIA RAM-DISCO
           Chiudo i mapping per ogni iterazione per evitare conflitti.
        */
        close_image(&blurred);
        close_image(&edge);
    }

    /* 5. RISULTATI
       Stampa tabellare: Threads | Media Blur | Media Edge
    */
    printf("%d%s%.6f%s%.6f\n",
           num_threads, SEP, blur_time / n_iter, SEP, edge_time / n_iter);

    return 0;
}

/**
 * --- TEST_FUNCTIONS ---
 * Verifica rapida della correttezza del codice su un singolo passaggio.
 */
int test_functions(netpbm *image, int num_threads) 
{
    netpbm blurred, edge;
    int err;

    omp_set_num_threads(num_threads);

    err = blur_image(image, BLURRED, &blurred);
    if (err != 0) 
    {
        fprintf(stderr, "Errore durante l'esecuzione di blur_image: %d\n", -err);
        return err;
    }

    err = edge_highlight(image, EDGE, &edge);
    if (err != 0) 
    {
        fprintf(stderr, "Errore durante l'esecuzione di edge_highlight: %d\n", -err);
        close_image(&blurred);
        return err;
    }

    close_image(&blurred);
    close_image(&edge);
    return 0;
}

/**
 * --- MAIN ---
 * Apre l'immagine sorgente e avvia prima il test e poi il benchmark completo.
 */
int main(void) 
{
    netpbm image;
    int err;

    /* 1. APERTURA HARDWARE
       Collego kitty.pgm tramite mmap.
    */
    err = open_image(REFERENCE, &image);
    if (err != 0) 
    {
        fprintf(stderr, "Unable to open source image: %d\n", -err);
        return 1;
    }

    /* 2. CONFIGURAZIONE AMBIENTE
       Fisso i thread per evitare che il sistema li vari durante il test.
    */
    omp_set_dynamic(0);

    printf("threads%sblur_time%sedge_time\n", SEP, SEP);

    int nproc = omp_get_num_procs();

    /* 3. TEST DI CORRETTEZZA
       Verifico che i filtri producano i file senza crashare.
    */
    err = test_functions(&image, nproc);
    if (err != 0) 
    {
        close_image(&image);
        return 1;
    }

    /* 4. BENCHMARK DI PRESTAZIONE
       Eseguo il calcolo per ogni numero di thread (da 1 a N)
       per analizzare la scalabilità del mio codice OpenMP.
    */
    for (int i = 1; i <= nproc; i++) 
    {
        err = benchmark_images(&image, i, 10); // 10 iterazioni per thread
        if (err != 0) 
        {
            close_image(&image);
            return 1;
        }
    }

    /* 5. CHIUSURA SORGENTE */
    close_image(&image);
    return 0;
}