# C-Tree (N-ary Tree) vs Binary Search Tree: Performance Analysis

Questo progetto analizza come una struttura dati più complessa, l'**Albero N-ario (C-Tree)**, possa superare in termini di prestazioni reali un **Albero Binario di Ricerca (BST)** classico, grazie a una migliore gestione della memoria cache.

## 📂 Organizzazione dei File
Il progetto è suddiviso nei seguenti moduli:
* **tree.c / tree.h**: Implementazione standard di un albero binario (1 nodo = 1 chiave, 2 figli).
* **ctree.c / ctree.h**: Implementazione ottimizzata di un albero n-ario (1 nodo = array di $N$ chiavi, $N+1$ figli).
* **main.c**: Test di correttezza visiva e benchmark prestazionale con 1 milione di elementi.
* **makefile**: Automazione della compilazione con flag di ottimizzazione massima (`-O3`).

## 🚀 Analisi delle Performance
Il vantaggio del **C-Tree** non risiede nella complessità asintotica (entrambi sono $O(\log n)$), ma nell'efficienza hardware:

1.  **Cache Locality**: Nel C-Tree, le chiavi all'interno di un nodo sono memorizzate in un array contiguo. Quando la CPU carica una chiave, carica l'intero blocco nel "vagone" della cache. Questo riduce i *Cache Miss* rispetto all'albero binario, dove ogni nodo è un salto in un punto diverso della RAM.
2.  **Riduzione dei Salti (Altezza)**: Avendo $N=3$ chiavi per nodo, l'albero n-ario è molto più basso. Per trovare un elemento, il programma deve effettuare meno chiamate ricorsive (meno "salti" tra i nodi).



### Risultati del Benchmark (1M di elementi)
*I tempi dipendono dall'hardware, ma il C-Tree mostra costantemente un'efficienza superiore.*

| Struttura | Tempo di Ricerca (1000 query) | Speedup |
| :--- | :--- | :--- |
| **Binary Tree** | ~0.455 ms | 1x (Baseline) |
| **C-Tree (N=3)** | **~0.316 ms** | **~1.4x più veloce** |



## 🛠️ Dettagli Implementativi
L'albero n-ario gestisce l'ordinamento interno tramite:
- **Shifting Ordinato**: Durante l'inserimento, se il nodo non è pieno, le chiavi vengono spostate a destra per far spazio alla nuova chiave mantenendo l'ordine.
- **Navigazione N-aria**: La ricerca identifica l'intervallo corretto tra le chiavi del nodo per decidere in quale degli $N+1$ figli scendere.

## 💻 Istruzioni per l'uso
1. **Compilazione**: `make`
2. **Esecuzione**: `./main`
3. **Pulizia**: `make clean`