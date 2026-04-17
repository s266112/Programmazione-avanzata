# Esercitazione 06: Trasposizione di Immagini e Memory Mapping

Questo esercizio riguarda l'implementazione della trasposizione di immagini in formato **Netpbm (PGM)**. L'obiettivo principale è ottimizzare l'accesso ai dati su disco utilizzando la tecnica del **Memory Mapping (`mmap`)**, studiando come la gerarchia di memoria e la linearizzazione dei dati influenzino l'efficienza del software.

## 🚀 Obiettivi dell'Esercitazione
L'obiettivo centrale è superare i limiti del classico I/O basato su buffer (`read`/`write`), che comporta un elevato overhead dovuto al passaggio dei dati tra kernel space e user space. Ho implementato la gestione dell'immagine tramite:

1.  **Memory Mapping (`mmap`)**: Invece di caricare l'immagine in un array temporaneo, il file viene proiettato direttamente nello spazio di indirizzamento virtuale del processo. Questo permette alla CPU di accedere ai pixel come se fossero byte residenti in RAM, lasciando al Sistema Operativo il compito di gestire il caricamento delle pagine dal disco (Demand Paging).
2.  **Linearizzazione delle Coordinate**: Poiché la memoria fisica è un nastro monodimensionale, ho implementato la funzione `pixel_at` per tradurre le coordinate $2D (x, y)$ nell'indice lineare corretto. Questo calcolo tiene conto dell'**Header testuale** del file PGM, saltando i metadati iniziali tramite un `offset` calcolato dinamicamente a runtime.
3.  **Trasposizione Naive**: Un algoritmo che inverte la posizione dei pixel ($dst[y][x] = src[x][y]$). Questa operazione è ideale per osservare il comportamento della cache, poiché forza accessi non sequenziali alla memoria di destinazione.

## 📊 Analisi dell'Efficienza Hardware
L'organizzazione dei dati influenza drasticamente il lavoro del processore, con dinamiche simili a quelle osservate nella moltiplicazione di matrici:

* **Cache Locality**: Nella funzione `transpose_naive`, la lettura dall'immagine sorgente è sequenziale (Row-Major). Il processore può prevedere i dati successivi e caricarli preventivamente nella cache L1 (**Prefetching**), massimizzando la velocità.
* **Cache Miss**: A causa della trasposizione, la scrittura nella destinazione avviene per colonne. Ogni volta che passiamo al pixel successivo nella sorgente, ci spostiamo di un'intera riga nella destinazione. Se l'immagine è grande, questo causa continui "Cache Miss", rendendo l'operazione **Memory Bound**.



## 🛠️ Dettagli Tecnici
Per garantire la stabilità e la correttezza a basso livello, sono stati curati i seguenti aspetti:

* **Gestione dello Spazio (ftruncate)**: Prima di mappare il file di destinazione, è necessario riservare lo spazio fisico sul disco. Senza l'uso di `ftruncate`, il file risulterebbe di dimensione zero e il tentativo di scrittura tramite il puntatore di `mmap` causerebbe un segnale di **SIGBUS (Bus Error)**.
* **Sincronizzazione (MAP_SHARED)**: La mappatura è stata configurata come condivisa. Questo assicura che le modifiche fatte dalla CPU alla memoria virtuale vengano sincronizzate permanentemente sul disco dal kernel.
* **Ottimizzazione del Compilatore**: Nel Makefile è stato utilizzato il flag `-O3` per permettere al compilatore di ottimizzare i calcoli degli indirizzi all'interno dei cicli annidati.



## 📂 Struttura del Progetto
* **`netpbm.c` / `netpbm.h`**: Core della libreria per la gestione hardware-mapped e navigazione dei pixel.
* **`main.c`**: Driver del programma che coordina l'apertura di `CM1.pgm` e la generazione dell'output trasposto.
* **`makefile`**: Automazione del processo di build e gestione delle dipendenze.

## ⚙️ Istruzioni per la Compilazione
Per compilare ed eseguire il progetto:
```bash
make clean
make
./main