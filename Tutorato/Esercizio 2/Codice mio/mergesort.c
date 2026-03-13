#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merge.h"

/* ===== 5: MERGESORT ITERATIVO  (EXTRA)=====
Ordino un vettore senza usare la ricorsione, ma unendo 
sottovettori di dimensioni crescenti (1, 2, 4, 8...). */

void merge_sort(int * v, int len, void (*m) (int *, int, int *, int, int *))
{
  // Vettore temporaneo per appoggiare i risultati dei merge
  int * results = (int *) malloc(len * sizeof(int));
  
  // Dimensione dei sottovettori da fondere (inizia da 1)
  for (int curr_size = 1; curr_size < len; curr_size = 2 * curr_size)
  {
    // Scorro il vettore per fondere le coppie di sottovettori
    for (int left_start = 0; left_start < len - curr_size; left_start += 2 * curr_size)
    {
      // Calcolo i confini dei due pezzi da unire
      int mid = left_start + curr_size;
      int right_end = left_start + 2 * curr_size;
      
      // Se l'ultimo pezzo è più piccolo della dimensione corrente
      if (right_end > len)
      {
        right_end = len;
      }

      // Lunghezza dei due pezzi
      int n1 = mid - left_start;
      int n2 = right_end - mid;

      // Chiamo la funzione di merge (m) passata come argomento
      m(v + left_start, n1, v + mid, n2, results + left_start);
      
      // Copio il risultato parziale nel vettore originale
      for (int i = 0; i < (n1 + n2); i++)
      {
        v[left_start + i] = results[left_start + i];
      }
    }
  }

  free(results);
}

int main()
{
  // Qui aggiungo un piccolo test per vedere se ordina
  int test[] = {9, 3, 7, 5, 1, 4, 8, 2};
  int n = 8;

  printf("Vettore originale: 9 3 7 5 1 4 8 2\n");
  
  // Uso la versione branchless per l'ordinamento
  merge_sort(test, n, merge_branchless);

  printf("Vettore ordinato:  ");
  for (int i = 0; i < n; i++)
  {
    printf("%d ", test[i]);
  }
  printf("\n");

  return 0;
}