#include <stdlib.h>
#include <stdio.h>
#include "unrolled_linked_list.h"


/* --- 1.INIALIZZAZIONE --- 
Questa funzione crea il "contenitore" principale della lista. 
All'inizio la lista è vuota, quindi il puntatore alla testa (head) non punta a nulla.
*/

unrolled_linked_list ulst_make(void)
{
  // Alloco lo spazio per la struttura della lista
  unrolled_linked_list lst = (unrolled_linked_list) malloc(sizeof(struct _unrolled_linked_list));

  // Se l'allocazione è riuscita, inializzo la testa a NULL per evitare che la lista punti a spazzatura in memoria
  if (lst != NULL)
  {
    lst->head = NULL; 
  }

  // Ritorno il puntatore alla lista creata
  return lst;
}



/*  ---- 2.INSERIMENTO ---
Questa è la parte più importante. Segue questa logica: 
1. Controlla se il primo nodo ha un posto libero negli array KEYS;
2. Se c'è un posto (dove VALID è False), mette il numero li.
3. Se il nodo è pieno (o non esiste), crea un nuovo nodo, lo mette in cima e usa il primo slot.
*/

void ulst_add(unrolled_linked_list lst, int key)
{
  if (lst == NULL) return;

  // - FASE A: Sfutto lo "srotolamento". Prima di crare un nuovo nodo, cerco se c'e un posto libero nell'array del nodo in testa.
  if (lst->head != NULL)
  {
    for (int i = 0; i < UNROLLED_SIZE; i++)
    {
      // Se valid[i] è false, lo slot i-esimo è libero
      if (!lst->head->valid[i])
      {
        lst->head->keys[i] = key;
        lst->head->valid[i] = true;
        // Elemento inserito, esco dalla funzione (quindi non eseguo la FASE B).
        return; 
      }
    }
  }

  // FASE B: Se arrivo qui, serve un nuovo nodo. (Il primo nodo era completamente pieno [tutti i 16 posti occupati])
  unode new_node = (unode) malloc(sizeof(struct _unrolled_node));
  if (new_node == NULL) return;

  // Preparo il nuovo nodo: resetto tutti i flag 'valid' a false, altrimenti conterebbero valori casuali trovati nella RAM
  for (int i = 0; i < UNROLLED_SIZE; i++)
  {
    new_node->valid[i] = false;
  }

  // Inserisco il valore nel primissimo slot disponibile [lo 0] e lo attivo
  new_node->keys[0] = key;
  new_node->valid[0] = true;

  // Collego al nuovo nodo: punta alla testa vecchia e diventa la nuova testa
  new_node->next = lst->head;
  lst->head = new_node;
}



/* --- 3. RICERCA ---
La ricerca avviene in due fasi:
1. Scansione dei nodi (come in una lista normale)
2. Per ogni nodo, scansione dell'array interno (scansione lineare)
*/

bool ulst_search(unrolled_linked_list lst, int key)
{
  // Se la lista non esiste, il valore non può esserci
  if (lst == NULL) return false;

  unode current = lst->head;

  // Ciclo esterno: Scorro tutti i nodi della lista
  while (current != NULL)
  {
    // Ciclo interno: Guardo dentro al nodo attuale. UNROLLED_SIZE è 16, quindi controllo tutti i 16 posti.
    for (int i = 0; i < UNROLLED_SIZE; i++)
    {
      /* Un valore è giusto se e solo se: 
      - L'interrutore 'VALID' è true (il posto non è vuoto)
      - Il valore in 'KEYS' corrisponde alla mia chiave */
      if (current->valid[i] && current->keys[i] == key)
      {
        // Trovato! Interrompo tutto e restituisco vero
        return true;
      }
    }
    // Se non ho trovato nulla in questo nodo, passo al prossimo
    current = current-> next;
  }
  // Se arrivo alla fine di tutti i nodi senza aver fatto 'return true', significa che il valore non è persente nella lista
  return false;
}


/* --- 4. CANCELLAZIONE ---
Devo liberare tutta la memoria usata. 
Prima elimino tutti i nodi uno ad uno, poi la struttura principale
*/

void ulst_delete(unrolled_linked_list lst)
{
  if (lst == NULL) return;

  unode current = lst->head;
  while (current != NULL)

  {
    // ATTENZIONE: Devo salvarmi il puntatore al prossimo nodo PRIMa di liberare quello attuale, altrimenti perdo tutto.
    unode next_node = current->next;
    
    // Libero il nodo attuale
    free(current);
    
    // Mi sposto su quello che avevo salvato
    current = next_node;
  }

  // Infine, libero la struttura della lista
  free(lst);
}



/* --- 5. STAMPA ---
Questa funzione mostra visivamente la struttura della lista srotolata.
Ogni nodo è racchiuso tra le parentesi quadre [].
I posti vuoti dell'array vengono stampati come un punto '.',
mentre i posti occupati mostrano il numero contenuto in 'KEYS'
*/

void ulst_print(unrolled_linked_list lst)
{
  // Se la lista non esiste stampa il NULL
  if (lst == NULL) 
  {
    printf("ERRORE: La lista non esiste");
    return;
  }

  printf("(");
  unode current = lst->head;

  // Scorro tutti i nodi
  while (current != NULL) 
  {
    printf("[");

    // Per ogni nodo, scorro tutto l'array interno di dimensione 16
    for (int i = 0; i < UNROLLED_SIZE; i++) 
    {
      if (current->valid[i])
      
      {
        // Se il valore è valido, stampo il numero
        printf("%d", current->keys[i]);
      } 
      else 
      {
        // Se il posto è libero, stampo il punto
        printf(".");
      }

      // Aggiungo uno spazio tra gli elementi, tranne dopo l'ultimo
      if (i < UNROLLED_SIZE - 1) 
      {
        printf(" ");
      }
    }
    printf("]");

    // Se c'è un altro nodo dopo questo, aggiungo uno spazio separatore
    if (current->next != NULL) {
      printf(" ");
    }
    current = current->next;
  }
  printf(")");
}
