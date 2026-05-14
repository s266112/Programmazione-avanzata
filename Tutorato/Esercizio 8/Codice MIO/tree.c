#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "tree.h"


/*  --- 1. ALLOCAZIONE NODO (MAKE_NODE) ---
  Creo fisicamente lo spazio in memoria per un nuovo elemento dell'albero.
*/
tree make_node(int key, float val, tree left, tree right)
{
  // Chiedo al sistema operativo di riservare la memoria necessaria. 
  tree t = (tree) malloc(sizeof(struct _tree_node));
  
  // Riempio i campi del nodo con i dati e i collegamenti ricevuti. 
  t->key = key;
  t->val = val;
  t->left = left;
  t->right = right;
  
  // Restituisco l'indirizzo di memoria del nuovo "vagone". 
  return t;
}


/*  --- 2. INSERIMENTO (INSERT) ---
  Posiziono una nuova coppia chiave-valore rispettando l'ordine del BST.
*/
tree insert(tree t, int key, float val)
{
  // CASO BASE: Se il ramo è vuoto, creo qui il nuovo nodo.
  if (t == NULL) 
  {
    return make_node(key, val, NULL, NULL);
  }
  
  // Se la chiave esiste già, aggiorno solo il valore.
  if (key == t->key)      
  {
    t->val = val;
  } 
  
  // Se la chiave è minore, vado a sinistra.
  else if (key < t->key) 
  {
    
    t->left = insert(t->left, key, val);
  } 
  
  // Se la chiave è maggiore, vado a destra.
  else                  
  {
    t->right = insert(t->right, key, val);
  }
  
  // Restituisco la radice (potenzialmente aggiornata).
  return t;
}


/*  --- 3. RICERCA (SEARCH) ---
  Cerco una chiave e ne estraggo il valore tramite puntatore.
*/
bool search(tree t, int key, float * val)
{
  // 1. NON TROVATO: Se arrivo a NULL, la chiave non è nell'albero. 
  if (t == NULL) 
  {
    return false;
  }

  // 2. TROVATO: Se trovo la chiave, scrivo il valore nella "scatola" puntata da 'val' e restituisco vero. 
  if (t->key == key) 
  {
    *val = t->val;
    return true;
  }

  // 3. DISCESA: Scelgo il ramo corretto in base alla chiave.
  if (key < t->key) 
  {
    return search(t->left, key, val);
  } 
  return search(t->right, key, val);
}


/* --- 4. DISTRUZIONE (DESTROY) ---
  Libero tutta la memoria occupata dall'albero.
*/
void destroy(tree t)
{
  // Se il nodo è nullo, non ho nulla da fare. 
  if (t == NULL) 
  {
    return;
  }

  // Elimino prima i figli (post-order traversal) e infine il padre. È fondamentale per non perdere i riferimenti in memoria. 
  destroy(t->left);
  destroy(t->right);
  
  // Distruggo fisicamente il nodo corrente. 
  free(t);
}