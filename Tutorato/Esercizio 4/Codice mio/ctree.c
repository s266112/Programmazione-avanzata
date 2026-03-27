#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "ctree.h"

/* --- 1. COSTRUZIONE ---
Funzione di supporto serve a creare un nuovo nodo quando quello attuale è pieno o quando l'albero è ancora vuoto
*/
ctree make_cnode(int key, float val)
{
  // Chiedo memoria al computer per contenere la struttura definita in ctree.h
  ctree n = (ctree) malloc(sizeof(struct _ctree_node));

  // Controllo sempre che la malloc sia andata a buon fine
  if (n != NULL)
  {
    // 1. Inizializzo il nodo con la prima coppia chiave-valore
    n->key[0] = key;
    n->val[0] = val;

    // 2. Questo contatore dice quanto è pieno il nodo.
    n->first_free = 1;

    /* 3. Iniziallizzo tutti i puntatori ai figli a NULL
        - Un nodo con N chiavi può avere fino a N+1 figli.
        - Devo "pulire" la memoria per evitare che punti ad indirizzi casuali
    */
    for (int i = 0; i < N + 1; i++)
    {
      n->children[i] = NULL;
    }
  }

  // Ritorno il nuovo puntatore
  return n;
}



/*  --- 2. INSERIMENTO ---
Questa funzione aggiunge una chiave e un valore. 
Se la funzione esiste gia, aggiorna il valore; se il nodo è pieno, scende nel figlio corretto.
*/
ctree cinsert(ctree t, int key, float val)
{
  // CASO A: L'albero è completamente vuoto
  if (t == NULL)
  {
    // Creo il primo nodo "radice", usando la funzione supporto.
    return make_cnode(key, val);
  }

  /* CASO B: La chiave esiste nel nodo corrente?
  In questo caso, scorro tutte le chiavi per vedere se devo aggiornare solo il valore */
  for (int i = 0; i < t->first_free; i++)
  {
    if (t->key[i] == key)
    {
      t->val[i] = val;
      return t;    // Finito, esco dal nodo
    }
  }

  // CASO C: Il nodo ha ancora SPAZIO LIBERO (first_free < N)
  if (t->first_free < N)
  {
    int pos = 0;
    // Troviamo la posizione esatta dove inserire per mantenere l'ordine
    while (pos < t->first_free && t->key[pos] < key) {
      pos++;
    }

    // Spostiamo tutto (chiavi, valori e figli) di un posto a destra per fare spazio
    for (int j = t->first_free; j > pos; j--) 
    {
      t->key[j] = t->key[j-1];
      t->val[j] = t->val[j-1];
      t->children[j+1] = t->children[j];
    }

    // Inseriamo i nuovi dati nella posizione "pos"
    t->key[pos] = key;
    t->val[pos] = val;
    // Il nuovo figlio a destra della nuova chiave deve essere inizialmente NULL
    t->children[pos+1] = NULL; 

    t->first_free++;
    return t;
  }
  

  /* CASO D: Il nodo è PIENO!
        - Devo decidere in quale figlio andare
        - Cerco la posizione corretta confrontando la chiave con quella del nodo
  */
  int i = 0;
  while (i < t->first_free && key > t->key[i])
  {
    i++;
  }

  // Ora 'i' è l'indice del figlio giusto (children[i]). Chiamo la funzione in modo ricorsivo su quel figlio.
  t->children[i] = cinsert(t->children[i], key, val);

  return t;
}



/*  --- 3. RICERCA ---
Questa funzione cerca una chiave nell'albero.
Se la trova, salva il valore in *val e restituisce true, altrimente false.
*/
bool csearch(ctree t, int key, float * val)
{
  // CASO A: L'albero (o il ramo) è vuoto
  if (t == NULL)
  {
    return false;
  }

  /* CASO B: La chiave è in questo nodo?
  Scorro le chiavi presenti nel nodo attuale */
  for (int i = 0; i < t->first_free; i++)
  {
    if (t->key[i] == key)
    {
      // Trovata! Salvo il valore nell'indirizzo puntato da val
      *val = t->val[i];
      return true;
    }
  }

  /* CASO C: La chiave non è qui, devo scendere nel figlio giusto
  Uso la stessa logica dell'inserimento per trovare il ramo giusto */
  int i = 0;
  while (i < t->first_free && key > t->key[i])
  {
    i++;
  }

  // Chiamo la ricerca in modo ricorsivo sul figlio individuato
  return csearch(t->children[i], key, val);
}



/*  --- 4. STAMPA ---
  - La funzione stampa l'albero in modo ricorsivo
  - Il formato segue la struttura: (figlio_0 chiave_0; figlio_1 chiave_1 ... figlio_n)
  - I rami vuoti sono indicati con un punto "."
*/
void print_ctree(ctree t)
{

  // Se il nodo è vuoto, stampo solo un punto (come negli alberi binari)
  if (t == NULL)
  {
    printf(".");
    return;
  }

  // Iniziallizo la rappresentazione dei nodi con una parentesi
  printf("(");

  // Devo stampare i figli e le chiavi in modo alternato. Se ho k chiavi, ho k+1 figli.
  int i;
  for (i = 0; i < t->first_free; i++)
  {
    // 1. Stampo il figlio che precede la chiave
    print_ctree(t->children[i]);

    // 2. Stampo la coppia chiave e valore
    printf(" %d:%0.1f ", t->key[i], t->val[i]);
  }
  // 3. Importante: stampo l'ultimo figlio (quello alla DX dell'ultima chiave)
  print_ctree(t->children[i]);

  // Chiudo la parentesi del nodo
  printf(")"); 
}
 