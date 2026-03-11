# Esercitazione 01: Albero Binario di Ricerca Modulare

Questo esercizio riguarda la scomposizione di un file monolitico in un progetto C modulare, con l'implementazione di una libreria personalizzata.

## 📂 Organizzazione dei File
L'implementazione è suddivisa nei seguenti moduli: 
* **tree_node**: Gestione della struttura base e della memoria del singolo nodo. 
* **bst**: Logica dell'albero binario (inserimento, calcolo profondità e distruzione). 
* **print_tree**: Funzione per la stampa testuale della struttura dell'albero. 
* **main**: Test delle performance tra inserimento sequenziale e casuale. 


## 🛠️ Compilazione (Makefile)
Il Makefile gestisce la creazione della libreria `libbst` in due modalità: 

1. **Libreria Statica**: 
   Compila tutto in un unico blocco.
   `make` 

2. **Libreria Dinamica**: 
   Crea una libreria condivisa (.so).
   `make SHARED=1` 

3. **Pulizia**: 
   Rimuove i file intermedi e l'eseguibile.
   `make clean` 

## 🚀 Esecuzione
Una volta compilato, avvia il programma con:
```bash
./main