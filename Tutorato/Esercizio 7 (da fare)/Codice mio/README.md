# Esercitazione 07: Filtri Immagine, Prefix Sum e Calcolo Parallelo

Questo esercizio riguarda l'implementazione di filtri di trasformazione spaziale (**Box Blur** e **Edge Highlight**) su immagini in formato **PGM**. L'obiettivo principale è l'ottimizzazione del calcolo tramite la tecnica della **Prefix Sum** e la parallelizzazione del carico di lavoro mediante **OpenMP**, mantenendo l'efficienza nell'accesso ai dati tramite **Memory Mapping (`mmap`)**.

## 🚀 Obiettivi dell'Esercitazione
L'obiettivo centrale è abbattere il costo computazionale tipico dei filtri a finestra mobile, che normalmente scalerebbe con il quadrato del raggio del filtro. Ho implementato la soluzione basandola su tre pilastri:

1.  **Tabella delle Somme Cumulative (Prefix Sum)**: Per evitare di ricalcolare la somma dei pixel per ogni finestra, ho implementato una fase di pre-processing che genera una matrice $P$ di dimensioni $(W+1) \times (H+1)$. Questa struttura permette di ottenere la somma di qualsiasi area rettangolare in tempo costante $O(1)$ tramite la formula $D - C - B + A$.
2.  **Parallelismo Multi-thread (OpenMP)**: L'elaborazione dell'immagine di destinazione è stata parallelizzata distribuendo i cicli sulle righe. Questo permette di saturare i core della CPU, riducendo il tempo di esecuzione proporzionalmente al numero di thread disponibili, evitando al contempo race conditions poiché ogni thread scrive su porzioni indipendenti di memoria mappata.
3.  **Gestione Hardware-Mapped (`mmap`)**: Seguendo la metodologia delle esercitazioni precedenti, il file viene proiettato nella memoria virtuale. Questo approccio elimina l'overhead delle chiamate di sistema di lettura/scrittura e permette al sistema operativo di gestire l'I/O tramite il meccanismo di Demand Paging.

## 📊 Analisi dell'Efficienza e Scalabilità
L'introduzione della Prefix Sum trasforma il problema da **CPU Bound** a **Memory Bound**. Di seguito i risultati del benchmark raccolti variando il numero di thread:

| Threads | Blur Time (s) | Edge Highlight Time (s) |
| :--- | :--- | :--- |
| 1 | 0.042281 | 0.030712 |
| 2 | 0.028217 | 0.021129 |
| 4 | 0.021688 | 0.018104 |
| 8 | 0.020232 | 0.016925 |
| 12 | 0.024206 | 0.023181 |

Dall'analisi dei tempi si osserva che lo speedup è significativo fino al raggiungimento del numero di core fisici. Oltre tale soglia (8-12 thread), l'incremento delle prestazioni decresce a causa dell'overhead di gestione della libreria OpenMP e della saturazione della banda passante verso la memoria RAM.

## 🛠️ Dettagli Tecnici e Robustezza
Per garantire la correttezza del software a basso livello e prevenire errori hardware, sono state adottate le seguenti precauzioni:

* **Prevenzione Overflow**: La tabella Prefix Sum utilizza tipi a 64-bit (`unsigned long long`) per contenere somme che eccederebbero i limiti dei classici interi a 32-bit.
* **Prenotazione Spazio (`ftruncate`)**: Il file di output viene dimensionato prima della mappatura. Questo previene segnali di **SIGBUS** durante la scrittura dei pixel trasformati.
* **Gestione dei Tipi e Cast**: Nel filtro di evidenziazione bordi, è stato applicato un cast esplicito a `(unsigned char)` per gestire correttamente la saturazione dei bianchi (255) ed evitare warning di overflow durante la conversione da interi.
* **Header Offset**: Il calcolo degli indirizzi tramite `pixel_at` e gli accessi diretti tengono conto dell'offset dinamico dell'header PGM, garantendo che i dati binari non vengano corrotti dai metadati testuali.

## 📂 Struttura del Progetto
* **`netpbm.c` / `netpbm.h`**: Libreria per la gestione del mapping hardware e l'accesso lineare ai dati.
* **`image_transform.c` / `image_transform.h`**: Modulo contenente l'algoritmo Prefix Sum e i filtri di trasformazione.
* **`main.c`**: Driver per la validazione funzionale e l'esecuzione dei benchmark prestazionali.
* **`makefile`**: Automazione del build con flag di ottimizzazione `-O3` e supporto `-fopenmp`.

## ⚙️ Istruzioni per la Compilazione
```bash
make clean
make
./main