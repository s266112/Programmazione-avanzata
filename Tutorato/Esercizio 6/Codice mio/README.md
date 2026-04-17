# Esercitazione 06: Trasposizione di Immagini e Memory Mapping

Questo esercizio riguarda l'implementazione della trasposizione di immagini in formato **Netpbm (PGM)**. L'obiettivo principale è ottimizzare l'accesso ai dati su disco utilizzando la tecnica del **Memory Mapping (`mmap`)**, analizzando come la gerarchia di memoria e la linearizzazione dei dati influenzino l'efficienza del software.

## 🚀 Obiettivi dell'Esercitazione
L'obiettivo centrale è superare i limiti del classico I/O basato su buffer (`read`/`write`), che comporta un elevato overhead dovuto al passaggio dei dati tra kernel space e user space. Ho implementato la gestione dell'immagine tramite:

1.  **Memory Mapping (`mmap`)**: Il file viene proiettato direttamente nello spazio di indirizzamento virtuale del processo. Questo permette alla CPU di accedere ai pixel come se fossero residenti in RAM, lasciando al Sistema Operativo il compito di gestire il caricamento delle pagine (Demand Paging).
2.  **Linearizzazione delle Coordinate**: Poiché la memoria fisica è un nastro monodimensionale, ho implementato la funzione `pixel_at` per tradurre le coordinate $2D (x, y)$ nell'indice lineare $1D$. Il calcolo tiene conto dell'**Header testuale** del file PGM, saltando i metadati tramite un `offset` dinamico.
3.  **Trasposizione Naive**: Un algoritmo che inverte la posizione dei pixel ($dst[y][x] = src[x][y]$). Questa operazione è ideale per osservare il comportamento della cache, poiché forza accessi non sequenziali nella memoria di destinazione.

## 📊 Analisi dell'Efficienza Hardware
L'organizzazione dei dati influenza drasticamente il lavoro del processore, con dinamiche simili a quelle osservate nella moltiplicazione di matrici:

* **Cache Locality**: Nella lettura dall'immagine sorgente, l'accesso è sequenziale (Row-Major), permettendo al processore di sfruttare il **Prefetching** della cache L1.
* **Cache Miss**: Nella scrittura dell'immagine di destinazione, l'accesso avviene "a salti" (Column-Major). Se l'immagine è grande, questo causa continui "Cache Miss", rendendo l'operazione **Memory Bound** (limitata dalla velocità della RAM e non della CPU).



## 🛠️ Dettagli Tecnici e Robustezza
Per garantire la stabilità del sistema e la correttezza a basso livello, ho implementato diverse soluzioni avanzate:

* **Gestione POSIX (`_DEFAULT_SOURCE`)**: Ho utilizzato macro specifiche per sbloccare funzioni di sistema come `ftruncate` e `fileno`, necessarie per manipolare correttamente i descrittori di file.
* **Prenotazione Spazio (`ftruncate`)**: Prima della mappatura, il file di output viene fisicamente espanso sul disco. Senza questo passaggio, il tentativo di scrivere pixel tramite `mmap` causerebbe un segnale di **SIGBUS (Bus Error)**.
* **Error Handling**: Il codice integra una gestione degli errori basata su `perror()`, che permette di diagnosticare istantaneamente fallimenti hardware o di permessi durante l'apertura o la mappatura dei file.
* **Sicurezza di Memoria**: La funzione `pixel_at` include un controllo dei bordi per prevenire tentativi di accesso a indirizzi non validi (**Segmentation Fault**).

## 📂 Struttura del Progetto
* **`netpbm.c` / `netpbm.h`**: Core della libreria per la gestione hardware-mapped e navigazione dei pixel.
* **`main.c`**: Driver del programma che coordina l'apertura, la creazione e la trasposizione delle immagini.
* **`makefile`**: Automazione del processo di build con ottimizzazioni `-O3`.

## ⚙️ Istruzioni per la Compilazione
```bash
make clean
make
./main