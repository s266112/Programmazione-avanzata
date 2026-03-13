# Esercitazione 02: Merge e Mergesort Branchless

Questo esercizio riguarda l'implementazione e l'analisi prestazionale di algoritmi di fusione (Merge), confrontando l'approccio standard con una tecnica avanzata definita "Branchless".

## 🚀 Obiettivi dell'Esercitazione
L'obiettivo principale è osservare come le decisioni del processore influenzino la velocità del codice. Ho implementato due varianti della funzione di Merge:

1. **Merge Standard**: Utilizza i classici costrutti `if-else` per decidere quale elemento inserire nel vettore finale.
2. **Merge Branchless**: Utilizza operatori logico-matematici per eliminare i salti condizionali (`if`), permettendo al processore di eseguire le istruzioni in modo lineare.

## 📊 Analisi delle Performance
Eseguendo i test contenuti nel `main`, ho analizzato i tempi medi di esecuzione su vettori di dimensioni crescenti (da 1.000 a 20.000 elementi). 

**Risultati osservati:**
* La versione **Branchless** risulta sensibilmente più veloce.
* Questo accade perché ho forzato il compilatore (tramite flag nel Makefile) a non ottimizzare i salti. 
* Senza ottimizzazioni, il processore subisce meno rallentamenti dovuti alla **Branch Misprediction** (errori di previsione dei rami), che nella versione standard causano lo svuotamento della pipeline di calcolo.

## 🛠️ Funzionalità Extra
Ho implementato inoltre un **Mergesort iterativo** (approccio bottom-up). A differenza della versione ricorsiva, questa funzione:
* Ordina il vettore partendo da coppie di elementi e fondendoli in blocchi sempre più grandi (2, 4, 8...).
* Utilizza le funzioni di merge sviluppate in precedenza, dimostrando come la velocità del "motore" di fusione influenzi l'intero processo di ordinamento.

## 📂 Esecuzione
Per compilare ed eseguire i test di performance:
```bash
make
./main