# Unrolled Linked List vs Linked List: Performance Analysis

Questo progetto analizza come strutture dati con la stessa complessità asintotica **O(n)** possano avere performance reali drasticamente differenti grazie all'ottimizzazione della memoria e all'uso efficiente della cache.

## 📂 Organizzazione dei File
Il progetto è suddiviso nei seguenti moduli:

* **linked_list**: Implementazione standard (un nodo = un elemento).
* **unrolled_linked_list**: Implementazione ottimizzata (un nodo = un array di 16 elementi).
* **main**: Test visivo di correttezza e benchmark con 10 milioni di elementi.
* **makefile**: Automazione della compilazione con flag di ottimizzazione massima (`-O3`).

## 🚀 Analisi delle Performance
Il vantaggio della **Unrolled Linked List** deriva da una migliore interazione con l'hardware della CPU:

1.  **Cache Locality**: Una *Cache Line* moderna è tipicamente di 64 byte. Poiché 16 interi occupano esattamente 64 byte, la CPU carica un intero "vagone" della lista in un unico ciclo di lettura. Questo riduce i *Cache Miss* rispetto alla lista classica, dove i nodi sono sparsi in posizioni casuali della RAM.
2.  **Riduzione Overhead di Allocazione**: Per 10 milioni di elementi, la lista classica effettua 10.000.000 di chiamate a `malloc()`. La Unrolled ne effettua solo 625.000, riducendo il carico sul sistema operativo e la frammentazione della memoria.

### Risultati di Esempio (Test su 10M di elementi)
*Nota: I tempi sono indicativi e variano in base all'hardware, ma il rapporto di efficienza rimane costante.*

| Struttura | Tempo di Ricerca (ms) | Speedup Relativo |
| :--- | :--- | :--- |
| **Linked List Classica** | ~275.83 ms | 1x (Baseline) |
| **Unrolled Linked List** | **~41.23 ms** | **~6.7x più veloce** |



## 🛠️ Dettagli Implementativi
L'inserimento nella Unrolled List gestisce dinamicamente lo spazio in due fasi:
- **Inserimento in testa**: Se il primo nodo ha slot liberi (verificati tramite l'array booleano `valid`), il dato viene inserito nell'array esistente senza nuove allocazioni.
- **Espansione**: Solo quando tutti i 16 slot del nodo in testa sono occupati, viene allocato un nuovo nodo e aggiunto alla lista.

## 💻 Istruzioni per l'uso
1. **Compilazione**: `make`
2. **Esecuzione**: `./main`
3. **Pulizia**: `make clean`

