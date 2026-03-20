#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"

// Crea il contenitore della lista standard
linked_list lst_make(void)
{
  linked_list lst = (linked_list) malloc(sizeof(struct _linked_list));
  lst->head = NULL;
  return lst;
}

// Elimina la lista nodo per nodo
void lst_delete(linked_list lst)
{
  if (lst == NULL) 
  {
    return;
  }
  node current = lst->head;
  while (current != NULL) 
  {
    node prev = current;
    current = current->next;
    // Libera i nodi uno ad uno
    free(prev);
  }
  free(lst);
}

// Inserimento classico: crea sempre un nuovo nodo (costoso per la CPU)
void lst_add(linked_list lst, int key)
{
  if (lst == NULL) 
  {
    return;
  }

  // Viene fatta una malloc per OGNI numero aggiunto.
  node new = (node) malloc(sizeof(struct _node));
  new->key = key;
  new->next = lst->head;
  lst->head = new;
}

// Ricerca lineare standard
bool lst_search(linked_list lst, int key)
{
  if (lst == NULL) 
  {
    return false;
  }
  node current = lst->head;
  // Ricerca classica : deve saltare da un indirizzo di memoria all'altro
  while (current != NULL) 
  {
    if (current->key == key) 
    {
      return true;
    }
    current = current->next;
  }
  return false;
}

// Stampa semplice della lista
void lst_print(linked_list lst)
{
  if (lst == NULL) 
  {
    printf("NIL");
    return;
  }
  printf("(");
  node current = lst->head;
  while (current != NULL) 
  {
    printf("%d", current->key);
    if (current->next != NULL) 
    {
      printf(" ");
    }
    current = current->next;
  }
  printf(")");
}
