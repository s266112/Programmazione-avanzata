# Python 2D Retro Renderer 🎮

## Descrizione

Questo progetto implementa un piccolo renderer grafico 2D in stile retro scritto interamente in Python. Il sistema gestisce una palette indicizzata di 16 colori, disegna un fondale a griglia tramite una tile map e sovrappone una serie di sprite con supporto alla trasparenza. Il risultato finale della composizione viene esportato in formato PNG.

## Funzionalità principali ✨

- Gestione di una palette a 16 colori RGB caricata da JSON
- Decodifica di file binari 4-bit packed per la memoria video (VRAM)
- Rendering del fondale tramite una tile map di 20x15 tile (32x32 pixel ciascuno)
- Gestione di sprite 64x64 pixel su griglia 4x4
- Trasformazioni matematiche: ribaltamenti (`flip_h`, `flip_v`) e rotazioni (0°, 90°, 180°, 270°)
- Clipping geometrico per ritagliare correttamente gli sprite sui bordi dello schermo
- Esportazione nativa in formato PNG indicizzato (modalità 'P' di Pillow)

## Requisiti 🛠️

Il software fa uso di librerie esterne per l'elaborazione matriciale ad alte prestazioni e per il salvataggio dell'immagine finale.

Per installare le dipendenze necessarie:

```bash
pip install numpy pillow
```

## Esecuzione ▶️

Il renderer viene eseguito da riga di comando passandogli i percorsi di tutti gli asset necessari per comporre la scena. L'ordine degli argomenti è rigido e predefinito.

Esempio di esecuzione con i file di test:

```bash
python main.py example/palette.json example/scene.json example/tiles.bin example/sprites.bin output.png
```

## Esempi forniti 📁

Nella cartella `example/` sono presenti gli asset forniti per testare il motore:
- `palette.json` – Definizione dei 16 colori disponibili
- `scene.json` – Struttura della mappa, indice di trasparenza e direttive di trasformazione sprite
- `tiles.bin` e `sprites.bin` – Texture in formato binario compresso a 4 bit

## Note implementative 🧠

- La libreria `numpy` gestisce i frame buffer tramite il tipo `np.uint8`, garantendo conversioni bit a bit rapidissime e un ridotto consumo di RAM.
- Il caricamento dei binari usa shift bit a bit (`>>` e `& 0x0F`) per separare i pixel.
- La libreria `Pillow` è utilizzata esclusivamente nell'ultimo passaggio di pipeline per codificare l'array 2D indicizzato nel file fisicamente compresso `.png`.

## Stato dei test ✅

Tutti i componenti modulari dell'architettura e la pipeline principale sono stati testati con successo sugli asset forniti. Le geometrie degli sprite (64x64) rispettano perfettamente i vincoli dell'area di disegno.

## Verifica utilizzata 🔍

L'architettura è stata sviluppata a moduli isolati. Comandi eseguiti per la validazione di ogni singola classe:

```bash
python palette.py example/palette.json
python vram.py example/tiles.bin example/sprites.bin
python scene_parser.py example/scene.json
python blitter.py example/scene.json example/tiles.bin example/sprites.bin
python image_exporter.py example/palette.json example/scene.json example/tiles.bin example/sprites.bin output.png
```

---

### Avvertenze ⚠️

- Il programma accetta input esclusivamente da argomenti riga di comando, non sono previste modalità interattive.
- I file JSON devono rispettare le chiavi standard (`transparent_index`, `tile_map`, `sprites`).
- Il file binario sorgente delle texture deve avere dimensione esatta di 32768 byte (256x256 pixel a 4-bit).