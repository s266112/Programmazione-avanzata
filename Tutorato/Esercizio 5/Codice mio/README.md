# Esercitazione 05: Moltiplicazione di Matrici e Cache Locality

Questo esercizio riguarda l'implementazione e l'analisi prestazionale di diverse varianti della moltiplicazione di matrici $n \times n$ (float), con l'obiettivo di ottimizzare l'uso della gerarchia di memoria (cache).

## 🚀 Obiettivi dell'Esercitazione
L'obiettivo principale è osservare come l'organizzazione dei dati in memoria influenzi drasticamente la velocità di calcolo. Ho implementato tre varianti della moltiplicazione:

1. **Moltiplicazione Standard (Naive)**: L'approccio classico riga per colonna che soffre di numerosi "Cache Miss" a causa degli accessi non contigui alla seconda matrice.
2. **Moltiplicazione Trasposta**: Utilizza una rappresentazione **Column Major** per la seconda matrice[cite: 99]. In questo modo, gli elementi della colonna diventano consecutivi in memoria, permettendo al processore di leggerli in modo lineare.
3. **Moltiplicazione a Blocchi (Tiling)**: Suddivide le matrici in blocchi di dimensione fissata ($16 \times 16$). Questa tecnica forza il processore a lavorare su porzioni di dati che possono risiedere stabilmente nella cache L1/L2, riducendo i tempi di attesa dalla RAM.

## 📊 Analisi delle Performance
Eseguendo i benchmark contenuti nel `main.c`, ho analizzato i tempi di esecuzione su matrici di grandi dimensioni ($1000 \times 1000$). 

**Risultati osservati:**
* **Standard Multiplication**: 781.382 ms (Baseline lenta a causa dei salti in memoria).
* **Transposed Multiplication**: 721.421 ms (Miglioramento grazie alla lettura sequenziale di B).
* **Blocked Multiplication**: **579.320 ms** (La versione più veloce grazie al riutilizzo della cache).

Questo accade perché, proprio come nel caso della *Branch Misprediction*, l'efficienza hardware dipende dalla capacità del codice di "prevedere" e preparare i dati correttamente per il processore.

## 🛠️ Dettagli Tecnici
Per garantire la massima efficienza e correttezza:
* **Gestione dei Bordi**: Ho implementato nel `kernel` dei controlli di sicurezza (`i < n`, `j < n`, `k < n`) per gestire i casi in cui la dimensione della matrice non è un multiplo esatto del blocco.
* **Flag di Compilazione**: Nel Makefile sono stati utilizzati i flag `-O3` e `-march=native` per permettere al compilatore di sfruttare al meglio le istruzioni specifiche della CPU.

## 📂 Esecuzione
Per compilare ed eseguire i test di correttezza e performance:
```bash
make
./main