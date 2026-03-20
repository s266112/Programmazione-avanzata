#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "linked_list.h"
#include "unrolled_linked_list.h"

// Dimensione della lista per il test di velocità
#define TEST_LIST_SIZE 10000000

/*--- TEST VISIVO: LISTA NORMALE ---
Inserisce 20 numeri casuali e stampa la lista ogni volta per vedere come cresce.
*/
void test_linked_list(void)
{
  linked_list lst = lst_make();
  for (int i = 0; i < 20; i++) 
  {
    // Aggiunge un numero tra 0 e 99
    lst_add(lst, rand()%100);

    // Mostra la lista nel terminale
    lst_print(lst);
    printf("\n");
  }
  // Libera la memoria alla fine del test
  lst_delete(lst);
}



/* --- TEST VISIVO: UNROLLED LIST --- 
   Fa la stessa cosa della precedente, ma usando la mia struttura a blocchi.
*/

void test_unrolled_linked_list(void)
{
  unrolled_linked_list lst = ulst_make();
  for (int i = 0; i < 20; i++) 
  {
    ulst_add(lst, rand()%100);

    // Qui vedrò gli array [ ] e i punti . che ho implementato
    ulst_print(lst);
    printf("\n");
  }
  ulst_delete(lst);
}



/* --- PERFORMANCE: LISTA NORMALE --- 
   Misura il tempo di ricerca in una lista classica.
*/

void perf_linked_list(int size, int repetitions)
{
  linked_list lst = lst_make();

  // Popolamento della lista con numeri casuali
  for (int i = 0; i < size; i++) 
  {
    lst_add(lst, rand());
  }

  // Inizio cronometro
  clock_t start = clock();
  for (int i = 0; i < repetitions; i++) 
  {
    // Cerca numeri a caso per 'repetition' volte
    lst_search(lst, rand());
  }
  // Fine cronometro
  clock_t end = clock();

  // Conversione della differenza di clock in millisecondi (ms)
  float ms = (end - start) / (CLOCKS_PER_SEC / 1000.0);
  printf("Searching in a linked list of %d elements %d times: %f ms\n", size, repetitions, ms);
  
  lst_delete(lst);
}



/* --- PERFORMANCE: UNROLLED LIST --- 
   Misura il tempo di ricerca nella MIA lista srotolata.
   È qui che mi aspetto il tempo più basso grazie alla località dei dati.
*/

void perf_unrolled_linked_list(int size, int repetitions)
{
  unrolled_linked_list lst = ulst_make();
  for (int i = 0; i < size; i++) 
  {
    ulst_add(lst, rand());
  }

  clock_t start = clock();
  for (int i = 0; i < repetitions; i++) 
  {
    // Usa la funzione di ricerca a doppio ciclo
    ulst_search(lst, rand());
  }
  clock_t end = clock();

  float ms = (end - start) / (CLOCKS_PER_SEC / 1000.0);
  printf("Searching in an unrolled linked list of %d elements %d times: %f ms\n", size, repetitions, ms);
  
  ulst_delete(lst);
}

int main(int argc, char * argv[])
{
  // Inizializza il seme per i numeri casuali
  srand(time(NULL));

  // Test visivo: prima la lista normale...
  printf("--- TEST VISIVO: LINKED LIST ---\n");
  test_linked_list();

  // 2. ...e poi la TUA lista srotolata con i vagoni [ . . . ]
  printf("\n--- TEST VISIVO: UNROLLED LINKED LIST ---\n");
  test_unrolled_linked_list(); 

  printf("\n--- TEST PRESTAZIONI (10 MILIONI DI ELEMENTI) ---\n");
  
  // 3. Performance lista classica
  perf_linked_list(TEST_LIST_SIZE, 10);
  
  // 4. Performance della TUA lista (la sfida finale)
  perf_unrolled_linked_list(TEST_LIST_SIZE, 10);

  return 0;
}
