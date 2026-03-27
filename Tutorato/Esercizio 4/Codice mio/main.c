#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "tree.h"
#include "ctree.h"

/* --- BENCHMARK ALBERO BINARIO ---
   Misura quanto tempo impiega l'albero binario a fare molte ricerche.
*/
void benchmark_tree(int n, int repetitions)
{
  tree t = NULL;
  // Riempie l'albero con 'n' elementi casuali
  for (int i = 0; i < n; i++) 
  {
    int k = rand();
    float v = (float) rand() / RAND_MAX;
    t = insert(t, k, v);
  }

  // Prepara un array di chiavi da cercare per il test
  int * to_search = (int *)malloc(repetitions * sizeof(int));
  for (int i = 0; i < repetitions; i++) 
  {
    to_search[i] = rand();
  }

  bool b = true;
  volatile bool tmp;    // Impedisce al compilatore di ottimizzare e saltare il ciclo
  float val;

  // Inizio misurazione tempo
  clock_t start = clock();
  for (int i = 0; i < repetitions; i++) 
  {
    b &= search(t, to_search[i], &val);   // Esegue la ricerca
  }
  clock_t end = clock();                  // Fine misurazione tempo

  tmp = b;
  // Converte i cicli di clock in millisecondi (ms)
  float ms = (float) (end - start) / (CLOCKS_PER_SEC / 1000.0);
  printf("Searching %d times in a tree: %f ms\n", repetitions, ms);
}


/* --- BENCHMARK C-TREE ---
   Identico al precedente, ma usa le tue funzioni dell'albero n-ario.
*/
void benchmark_ctree(int n, int repetitions)
{
  ctree t = NULL;
  for (int i = 0; i < n; i++) 
  {
    int k = rand();
    float v = (float) rand() / RAND_MAX;
    t = cinsert(t, k, v);
  }
  int * to_search = (int *)malloc(repetitions * sizeof(int));
  for (int i = 0; i < repetitions; i++) 
  {
    to_search[i] = rand();
  }

  bool b = true;
  volatile bool tmp;
  float val;

  clock_t start = clock();
  for (int i = 0; i < repetitions; i++) 
  {
    b &= csearch(t, to_search[i], &val);
  }
  clock_t end = clock();

  tmp = b;
  float ms = (float) (end - start) / (CLOCKS_PER_SEC / 1000.0);
  printf("Searching %d times in a c-tree: %f ms\n", repetitions, ms);
}


/* --- TEST DI CORRETTEZZA ---
   Queste funzioni servono solo a verificare che gli alberi 
   funzionino bene (stampano la struttura e cercano numeri da 0 a 9).
*/
void test_tree(int n)
{
  tree t = NULL;
  for (int i = 0; i < n; i++) 
  {
    int k = rand() % n;
    float v = (float) rand() / RAND_MAX;
    t = insert(t, k, v);
      print_tree(t);
      printf("\n");
  }
  for (int i = 0; i < n; i++) 
  {
    float v;
    printf("Searching for %d: %c\n", i, search(t, i, &v) ? 't' : 'f');
  }
}

void test_ctree(int n)
{
  ctree t = NULL;
  for (int i = 0; i < n; i++) 
  {
    int k = rand() % n;
    float v = (float) rand() / RAND_MAX;
    t = cinsert(t, k, v);
    print_ctree(t);
    printf("\n");
  }
  for (int i = 0; i < n; i++) 
  {
    float v;
    printf("Searching for %d: %c\n", i, csearch(t, i, &v) ? 't' : 'f');
  }
}


/* --- FUNZIONE PRINCIPALE --- */
int main(int argc, char * argv[])
{ 
  // 1. Esegue prima i test visivi su piccola scala (10 elementi)
  test_tree(10);
  // 2. Esegue il benchmark pesante (1 milione di inserimenti, 1000 ricerche)
  benchmark_tree(1000000, 1000);

  // 3. Ripete gli stessi test per il tuo c-tree
  test_ctree(10);
  benchmark_ctree(1000000, 1000);
  
  return 0;
}
