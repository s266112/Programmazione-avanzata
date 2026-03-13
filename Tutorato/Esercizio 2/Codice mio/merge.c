#include "merge.h"

/*  ===== PARTE 1: MERGE STANDARD =====
Utilizza un ciclo while e istruzioni if-else per fondere due vettori ordinati.*/

void merge(int * v1, int n1, int * v2, int n2, int * results)
{
  /// Indici per scorrere i vettori
  int i = 0, j = 0, k = 0;

  // 1. INTUIZIONE: Confronto gli elementi "testa a testa" [cite: 27, 28]
  while (i < n1 && j < n2)
  {
    // Se l'elemento in v1 è più piccolo di quello in v2 [cite: 29]
    if (v1[i] < v2[j])
    {
      results[k] = v1[i];    // Copia da v1
      i++;                   // Avanzo in v1
    }
    else
    // Se l'elemento v2 è più piccolo o uguale a quello in v1 [cite: 30]
    {
      results[k] = v2[j];    // Copia da v2
      j++;                   // Avanzo in v2
    }
    
    k++;                     // Avanzo in results
  }

  // 2. STRUTTURA: Gestione dei rimanenti 
  while (i < n1)
  {
    results[k++] = v1[i++];
  }

  // Copio quello che resta di v2
  while (j < n2)
  {
    results[k++] = v2[j++];
  }
}

/* ======= PARTE 3: MERGE BRANCHLESS ======
    Questa versione non usa "if" nel ciclo principale per evitare rallentamenti 
    dovuti alla previsione dei rami del processore */

void merge_branchless(int * v1, int n1, int * v2, int n2, int * results)
{
  int i = 0, j = 0, k = 0;
  
  while (i < n1 && j < n2)
  {
    // Confronto matematico: Se v1[i] < v2[j], c = 1, altrimenti c=0.
    int c = (v1[i] < v2[j]);

    // Uso c per scegliere il valore senza fare if. Se c=1 -> v1[i] oppure c=0 ->v1[j]
    results[k] = c ? v1[i] : v2[j];

    /* Incremento l'indice usando matematica:
      - Se C=1, i aumenta di 1 e j aumenta di 0;
      - se c=0, i aumenta di 0 e J aumenta di 1 */
      i += c;
      j += (1 - c);

      k++;
  }

  // Uso i cicli normali come prima
  while (i < n1)
  {
    results[k++] = v1[i++];
  }
  while (j < n2)
  {
    results[k++] = v2[j++];
  }
}
