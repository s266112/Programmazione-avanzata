# 🧠 TensorForth: High-Performance Tensor Interpreter

**TensorForth** è un interprete di linguaggio *stack-oriented* (ispirato alla filosofia Forth) progettato per l'elaborazione efficiente di tensori multi-dimensionali. Il sistema combina la flessibilità di un linguaggio basato su stack con la potenza del calcolo parallelo in C, offrendo strumenti avanzati per l'algebra lineare e l'image processing.

---

## 🏗️ Architettura Tecnica e Innovazioni

Il progetto è stato sviluppato con un focus particolare sulle prestazioni e sulla gestione rigorosa delle risorse di sistema.

### 1. Sistema di Gestione della Memoria (Reference Counting)
Per massimizzare l'efficienza ed evitare allocazioni ridondanti, l'interprete implementa un sistema di **Reference Counting**:
* **Zero-Copy Logic**: Operazioni di manipolazione dello stack come `dup` o `over` non duplicano i dati fisici in memoria, ma incrementano il contatore dei riferimenti del tensore esistente.
* **Gestione Automatica**: La memoria viene liberata (`free`) solo quando l'ultimo riferimento a un tensore viene rimosso dallo stack (tramite `drop` o sovrascrittura), garantendo l'assenza di memory leak.

### 2. Calcolo Parallelo con OpenMP
Tutte le operazioni computazionalmente onerose sono state parallelizzate per sfruttare architetture multi-core:
* **Algebra Lineare**: Il prodotto matriciale (operatore `@`) e le operazioni elemento per elemento sono distribuiti su più thread.
* **Image Processing**: La convoluzione 2D (operatore `c`) utilizza una distribuzione del carico di lavoro ottimizzata per processare immagini ad alta risoluzione in tempo reale.
* **Riduzioni Sicure**: Operazioni come la somma totale (`S`) utilizzano clausole `reduction` di OpenMP per garantire risultati deterministici anche in ambienti paralleli.

### 3. Persistenza e Memory Mapping (I/O Avanzato)
Il sistema implementa un protocollo di I/O binario proprietario estremamente performante:
* **Memory Mapping (`mmap`)**: Grazie all'operatore `{`, i file binari vengono mappati direttamente nello spazio di indirizzamento virtuale del processo. Questo permette di accedere a tensori di grandi dimensioni senza il sovraccarico delle funzioni `read`/`write` standard.
* **64-Byte Custom Header**: Per garantire la portabilità e la coerenza dei dati, ogni file `.bin` utilizza un header strutturato:
    * **Byte 0-39**: Shape del tensore (fino a 10 dimensioni).
    * **Byte 40-43**: Numero effettivo di dimensioni (NDim).
    * **Byte 44-51**: Data Offset (fissato a 64 byte per garantire l'allineamento dei dati).
    * **Byte 52-63**: Spazio riservato per metadati futuri.

---

## 🛠️ Set di Operatori Implementati

L'interprete supporta una vasta gamma di operatori manipolabili tramite notazione postfissa (RPN):

| Categoria | Operatori | Descrizione |
| :--- | :--- | :--- |
| **Aritmetica** | `+`, `-`, `*` | Operazioni elemento per elemento tra tensori compatibili. |
| **Algebra** | `@` | Prodotto righe-per-colonne (Matrix Multiplication). |
| **Immagini** | `c`, `i`, `o` | Convoluzione 2D, caricamento e salvataggio file PGM. |
| **Persistenza** | `{`, `}` | Caricamento tramite `mmap` e salvataggio binario su disco. |
| **Logica** | `<`, `>`, `=` | Confronto elemento per elemento (con tolleranza epsilon). |
| **Stack** | `dup`, `drop`, `over`, `swap` | Manipolazione classica dei riferimenti nello stack. |
| **Utility** | `p`, `S`, `r`, `f`, `?` | Stampa, Somma totale, Reshape, Fill e Random generation. |

---

## 🚦 Suite di Test e Dimostrazioni

Il progetto include una serie di script `.tensorforth` (nella cartella `examples/`) per validare ogni componente:

### Elaborazione Digitale delle Immagini
* **Sfocatura**: `image_blur.tensorforth` utilizza un kernel 5x5 per dimostrare la potenza della convoluzione parallela.
* **Edge Detection**: `detect_edges.tensorforth` implementa un filtro di Laplace per l'estrazione dei contorni.
* **Binarizzazione**: `convert_to_bw.tensorforth` mostra l'uso degli operatori logici per trasformare un'immagine in bianco e nero.

### Stress Test e Validazione Dati
* **Persistenza Binaria**: `save_tensor.tensorforth` valida la scrittura e la rilettura tramite `mmap` confrontando i checksum dei dati.
* **Matrici Grandi**: `random_matmul.tensorforth` mette sotto sforzo il motore OpenMP con moltiplicazioni di matrici casuali.

---

## 🔧 Compilazione

Il progetto include un `Makefile` ottimizzato per ambienti Linux/POSIX.

```bash
# Per compilare l'eseguibile 'tensorforth'
make

# Per pulire i file temporanei e l'eseguibile
make clean