#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "tree.h"

/*  --- 1. COSTRUZIONE ---
Crea un nodo classico con UNA sola chiave e DUE figli. 
*/
tree make_node(int key, float val, tree left, tree right)
{
  // Alloco memoria per un singolo nodo (struttura definita in tree.h)
  tree t = (tree) malloc(sizeof(struct _tree_node));

  // Assegno i dati e i puntatori ai due rami (SN e DX)
  t->key = key;
  t->val = val;
  t->left = left;   
  t->right = right; 

  return t;
}


/* --- 2. INSERIMENTO RICORSIVO ---
   Decide dove posizionare la chiave scegliendo tra due sole strade.
*/
tree insert(tree t, int key, float val)
{
  // Se arrivo in un NULL, creo il nodo qua
  if (t == NULL) 
  {
    return make_node(key, val, NULL, NULL);
  }

  // Se la chiave esiste gia, aggiorno il valore
  if (key == t->key) 
  {
    t->val = val;
  } 

  // Se la chiave è minore, scendo a SINISTRA
  else if (key < t->key) 
  {
    t->left = insert(t->left, key, val);
  }
  
  // Se la chiave è maggiore, scendo a DESTRA
  else 
  {
    t->right = insert(t->right, key, val);
  }
  return t;
}


/* --- 3. RICERCA ---
   Scorre l'albero seguendo i bivi fino a trovare la chiave o un vicolo cieco (NULL).
*/
bool search(tree t, int key, float * val)
{
  // Se arrivo a NULL, la chiave non esiste nell'albero
  if (t == NULL) 
  {
    return false;
  }

  // Se la chiave del nodo attuale è quella cercata, ho finito
  if (t->key == key) 
  {
    *val = t->val;  // Restituisco il valore tramite il puntatore
    return true;
  }

  // Scelgo la direzione in base al valore della chiave
  if (key < t->key) 
  {
    return search(t->left, key, val);   // Cercvo nel ramo sinistro
  } 
  return search(t->right, key, val);    // Cerco nel ramo destro
}


/* --- 4. STAMPA ---
   Visualizza l'albero in forma testuale usando le parentesi.
*/
void print_tree(tree t)
{
  // Se il ramo è vuoto, stampo un punto
  if (t == NULL) 
  {
    printf(".");
    return;
  }


  printf("(");
  print_tree(t->left);    // Prima stampo tutto quello che c'è a sinistra

  // Stampo la chiave del nodo centrale
  printf(" %d ", t->key);


  print_tree(t->right);   // Poi stampo tutto quello che c'è a destra
  printf(")");
}
