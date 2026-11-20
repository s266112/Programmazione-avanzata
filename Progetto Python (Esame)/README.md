# Python 2D Retro Renderer 🎮

## Descrizione

Questo progetto implementa un piccolo renderer 2D in stile retro in Python. Il sistema usa una palette indicizzata di 16 colori, disegna il fondale tramite una tile map e sovrappone gli sprite con trasparenza. Il risultato viene convertito in RGB ed esportato in PNG.

## Funzionalità principali ✨

* Palette di 16 colori RGB caricata e validata da JSON
* Decodifica dei file binari 4-bit packed di tile sheet e sprite sheet
* Fondale tramite tile map 20x15 con tile 32x32
* Sprite 64x64 da uno sprite sheet 4x4
* Ribaltamenti `flip_h`, `flip_v` e rotazioni di 0°, 90°, 180°, 270°
* Trasparenza sugli sprite tramite `transparent_index`
* Clipping degli sprite che escono dallo schermo
* Conversione del frame buffer in RGB ed esportazione in PNG

## Requisiti 🛠️

Per installare le dipendenze:

```
pip install numpy pillow
```

## Avvio (WSL) 🐧

```
cd /mnt/c/Users/kokin/Desktop/Prog_Script/Programmazione-avanzata
source .venv_pap/bin/activate
cd "Progetto Python (Esame)"
```

Per pulire i file generati:

```
rm -f output.png
rm -rf __pycache__
```

## Esecuzione ▶️

Il renderer si esegue da riga di comando nella forma:

```
python3 main.py <palette.json> <scene.json> <tiles.bin> <sprites.bin> <output.png>
```

Esempio:

```
python3 main.py example/palette.json example/scene.json example/tiles.bin example/sprites.bin output.png
```

## Esempi forniti 📁

Nella cartella `example/` trovi gli asset per testare il renderer:

* `palette.json` – palette di 16 colori
* `scene.json` – tile map, indice di trasparenza e sprite con le trasformazioni
* `tiles.bin` – tile sheet 256x256 in formato 4-bit packed
* `sprites.bin` – sprite sheet 256x256 in formato 4-bit packed
* `reference.png` – immagine attesa per la scena di esempio

## Note implementative 🧠

* `Palette` – carica la palette e risolve un indice nel colore RGB
* `VirtualVRAM` – decodifica i nibble con `>> 4` (primo pixel) e `& 0x0F` (secondo pixel)
* `SceneParser` – carica e valida la scena (id dei tile 0..63, id degli sprite 0..15, rotazioni ammesse)
* `Blitter` – estrae tile e sprite, applica flip, poi rotazione (in senso orario), clipping e trasparenza
* `ImageExporter` – converte in RGB con `numpy` usando il frame buffer come indice nella palette
* `RenderingPipeline` (in `main.py`) – esegue tutta la composizione della scena
* Tutti gli array usano `np.uint8`, `Pillow` serve solo per salvare il PNG
* La trasparenza vale solo per gli sprite, non per il fondale
* Gli id dei tile vengono convertiti in `int` prima dei calcoli, per evitare l'overflow di `np.uint8` con NumPy 2

## Stato dei test ✅

L'immagine generata dalla scena di esempio coincide pixel per pixel con `example/reference.png`.

## Verifica utilizzata 🔍

Ogni modulo si può eseguire da solo per testare la propria classe:

```
python3 palette.py example/palette.json
python3 vram.py example/tiles.bin example/sprites.bin
python3 scene_parser.py example/scene.json
python3 blitter.py example/scene.json example/tiles.bin example/sprites.bin
python3 image_exporter.py example/palette.json example/scene.json example/tiles.bin example/sprites.bin output.png
python3 main.py example/palette.json example/scene.json example/tiles.bin example/sprites.bin output.png
```

## Avvertenze ⚠️

* Il programma accetta input solo da riga di comando, non ci sono modalità interattive
* La scena deve contenere `transparent_index`, `tile_map` e `sprites`; ogni sprite deve avere `id`, `x`, `y`, `flip_h`, `flip_v` e `rotation`
* `rotation` può valere solo 0, 90, 180 o 270
* I file binari devono essere di esattamente 32768 byte
* In caso di errore vengono sollevate eccezioni specifiche (`FileNotFoundError`, `ValueError`, `TypeError`, `KeyError`) e il programma termina con codice 1