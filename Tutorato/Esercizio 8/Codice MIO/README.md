# Esercitazione 09: Binary Search Tree (Hybrid Python/C)

## 🎯 OBIETTIVO DEL PROGETTO
L'obiettivo di questa esercitazione è lo sviluppo di un **Albero Binario di Ricerca (BST)** sfruttando un'architettura ibrida. Abbiamo implementato la struttura sia in modalità nativa Python che tramite il caricamento di una libreria dinamica scritta in C, comunicando attraverso il modulo `ctypes`.

---

## 📊 ANALISI LOGICA (STRUTTURA DATI)
L'albero è composto da nodi che seguono la regola fondamentale dei BST: per ogni nodo, le chiavi nel sotto-albero sinistro sono minori e quelle nel sotto-albero destro sono maggiori.

### 1. Modulo Python (`tree.py`)
* **Classe Node**: Rappresenta l'unità fondamentale. Contiene il carico utile (`key`, `value`) e i riferimenti logici ai figli (`left`, `right`).
* **Classe BinarySearchTree**: Funge da interfaccia manageriale. Gestisce la radice (`root`) e fornisce i metodi per interagire con l'intera struttura.
* **Classe CTree**: Implementa il "ponte" verso il C. Mappa la struttura hardware del nodo e carica la libreria `libtree.so`.

### 2. Modulo C (`tree.c`, `tree.h`)
* **Memoria Manuale**: La gestione dei nodi avviene a basso livello tramite `malloc` per la creazione e `free` per la distruzione.
* **Puntatori**: La navigazione dell'albero avviene tramite indirizzi di memoria, permettendo prestazioni superiori nella gestione di grandi moli di dati.

---

## 🛠️ DESCRIZIONE DELLE FUNZIONI

### Operazioni BST (Python & C)
* **`insert(key, value)`**: Inserisce un nuovo nodo o aggiorna il valore se la chiave è già esistente, mantenendo l'albero ordinato.
* **`search(key)`**: Ricerca ricorsiva che scende tra i bivi dell'albero. Restituisce il valore associato o `None` se la chiave non viene trovata.
* **`__str__` / `print`**: Genera una rappresentazione testuale gerarchica dell'albero utilizzando parentesi annidate.
* **`destroy` (C only)**: Funzione critica che libera ricorsivamente la memoria RAM per prevenire memory leak.

---

## 🚀 COMPILAZIONE ED ESECUZIONE

1. **COMPILAZIONE**: Generazione della libreria condivisa tramite Makefile.
   ```bash
   make