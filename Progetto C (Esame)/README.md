# TensorForth 😄

## Descrizione

Questo progetto implementa un interprete stack-based per tensori in C. Il linguaggio supporta tensori 1D/2D, operazioni aritmetiche, confronti, matrici, convoluzione, reshape e I/O su file PGM e binari.

## Funzionalità principali ✨

- Interpretazione token-based in stile Forth
- Stack di tensori con reference counting
- Operazioni elemento per elemento: `+`, `-`, `*`
- Confronti logici: `<`, `>`, `=`
- Operatori logici: `&`, `|`, `!`
- Matrici 2D: prodotto `@`, convoluzione `c`
- Ravel `~` / alias `_`, `shape` `#`, `reshape` `r`
- Generazione casuale `?`, fill `f`, somma riduzione `S`
- Gestione I/O file PGM: `(` per lettura, `)` per scrittura
- Persistenza binaria con `mmap`: `{` per lettura, `}` per scrittura

## Compilazione 🛠️

Per compilare l'eseguibile:

```bash
make
```

Per pulire i file generati:

```bash
make clean
```

## Esecuzione ▶️

L'interprete legge come input un file `.tensorforth` o dallo standard input.

Esempio:

```bash
./tensorforth examples/duplicate.tensorforth
```

## Esempi forniti 📁

Nella cartella `examples/` trovi script di prova che coprono diverse funzionalità:

- `duplicate.tensorforth` – test di duplicazione e somma
- `save_tensor.tensorforth` – test di salvataggio e lettura binaria con `mmap`
- `convert_to_bw.tensorforth` – binarizzazione immagine PGM
- `detect_edges.tensorforth` – rilevamento bordi su immagine PGM
- `image_blur.tensorforth` – sfocatura immagine con convoluzione
- `random_matmul.tensorforth` – moltiplicazione di matrici casuali

## Note implementative 🧠

- `(` legge un file PGM e normalizza i pixel in `[0.0, 1.0]`
- `)` scrive un tensore 2D in formato PGM `P5`
- `{` mappa un file binario in memoria tramite `mmap`
- `}` salva un tensore in un file binario con header fisso da 64 byte

## Stato dei test ✅

I file di esempio principali sono stati eseguiti con successo, quindi l'interprete funziona correttamente con i casi forniti in `examples/`.

## Verifica utilizzata 🔍

Comandi eseguiti per la validazione:

```bash
make
./tensorforth examples/duplicate.tensorforth
./tensorforth examples/save_tensor.tensorforth
./tensorforth examples/convert_to_bw.tensorforth
./tensorforth examples/detect_edges.tensorforth
./tensorforth examples/image_blur.tensorforth
./tensorforth examples/random_matmul.tensorforth
```

---

### Avvertenze ⚠️

- I file PGM devono essere in formato `P5` e con `max_val` corretto
- `reshape` richiede che il numero totale di elementi rimanga invariato
- `random` e `fill` richiedono forme valide e valori interi positivi
