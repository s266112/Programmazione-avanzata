#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "merge.h"

int cmpfunc (const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}

int * random_vector(int len)
{
  int * v = (int *) malloc (len * sizeof(int));
  for (int i = 0; i < len; i++) {
    v[i] = rand();
  }
  return v;
}

float test_merge(int n, void (* m) (int *, int, int *, int, int *))
{
  int * v1 = random_vector(n);
  int * v2 = random_vector(n);
  int * results = (int *) malloc(2 * n * sizeof(int));
  qsort(v1, n, sizeof(int), cmpfunc);
  qsort(v2, n, sizeof(int), cmpfunc);
  clock_t start = clock();
  m(v1, n, v2, n, results);
  clock_t end = clock();
  float ms = (float) (end - start) / (CLOCKS_PER_SEC / 1000.0);
  free(v1);
  free(v2);
  free(results);
  return ms;
}

float avg_test_merge(int n,
		    void (* m) (int *, int, int *, int, int *),
		    int repetitions)
{
  float avg = 0;
  for (int i = 0; i < repetitions; i++) {
    avg += test_merge(n, m);
  }
  return avg / repetitions;
}

void check_merge(void (* m) (int *, int, int *, int, int *))
{
  int a[5] = {4, 6, 9, 17, 20};
  int b[5] = {2, 5, 7, 10, 18};
  int c[10];
  m(a, 5, b, 5, c);
  for (int i = 0; i < 10; i++) {
    printf("%d ", c[i]);
  }
  printf("\n");
}

/* ===== EXTRA: MERGESORT ITERATIVO ===== */
void merge_sort(int * v, int len, void (*m) (int *, int, int *, int, int *))
{
  int * results = (int *) malloc(len * sizeof(int));
  for (int curr_size = 1; curr_size < len; curr_size = 2 * curr_size)
  {
    for (int left_start = 0; left_start < len - 1; left_start += 2 * curr_size)
    {
      int mid = left_start + curr_size;
      int right_end = (left_start + 2 * curr_size < len) ? (left_start + 2 * curr_size) : len;
      if (mid < len) 
      {
        m(v + left_start, mid - left_start, v + mid, right_end - mid, results + left_start);
        for (int i = left_start; i < right_end; i++)
        {
          v[i] = results[i];
        }
      }
    }
  }
  free(results);
}

int main(int argc, char * argv[])
{
  srand(time(NULL));
  
  /* ===== PARTE 3: TEST IMPLEMENTAZIONE =====
   Confrontare la versione Standard e quella Branchless*/

   // 1. Verifico la correttezza di entrambe
  printf("Verifica correttezza Standard (check_merge):\n");
  check_merge(merge);

  printf("\nVerifica correttezza Branchless (check_merge):\n");
  check_merge(merge_branchless);

  // 2. Test Performance: versione STANDARD
  printf("\nPerformance Test (Standard Merge):\n");
  printf("Dimensione\tTempo Medio (ms)\n");
  printf("----------------------------------\n");

  // Ciclo per testare le dimensioni da 1000 a 20000 
  for (int n = 1000; n <= 20000; n += 1000)
  {
    float tempo_medio = avg_test_merge(n, merge, 10);   // Uso la funzione del prof
    printf("%d\t\t%.4f\n", n, tempo_medio);            // Stampo
  }

  // 3. Test Performance: Versione BRANCHLESS
  printf("\nPerformance Test (Branchless Merge):\n");
  printf("Dimensione\tTempo Medio (ms)\n");
  printf("----------------------------------\n");

  for (int n = 1000; n <= 20000; n += 1000)
  {
    float tempo_medio = avg_test_merge(n, merge_branchless, 10);
    printf("%d\t\t%.4f\n", n, tempo_medio);
  }

  // 4. Test Performance: Versione MERGESORT
  printf("\n--- Test Extra: Mergesort su vettore disordinato ---\n");
  int dati[] = {9, 1, 8, 2, 7, 3, 6, 4, 5};
  int n_extra = 9;
  merge_sort(dati, n_extra, merge_branchless);
  printf("Vettore ordinato: ");
  for(int i=0; i<n_extra; i++) printf("%d ", dati[i]);
  printf("\n");

   return 0;
}
