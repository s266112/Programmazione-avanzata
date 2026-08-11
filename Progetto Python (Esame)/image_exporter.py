# Cognome: Matessi
# Nome: Alessio
# Matricola: SM3201657

"""image_exporter.py

Gestisce l'esportazione del frame buffer indicizzato
in un file immagine (PNG) utilizzando la libreria Pillow e la palette.
"""

from PIL import Image
import numpy as np

class ImageExporter:
    def __init__(self, palette) -> None:
        # Salvo il riferimento alla palette per poter applicare i colori
        self.palette = palette

    def export(self, frame_buffer: np.ndarray, output_path: str) -> None:
        # Controllo che il frame buffer sia bidimensionale (480x640)
        if frame_buffer.ndim != 2:
            raise ValueError("Il frame buffer deve essere una matrice 2D.")

        # Creo un'immagine in modalità 'P' (Palette) partendo dalla matrice numpy
        img = Image.fromarray(frame_buffer, mode='P')

        # Pillow richiede che la palette sia una lista "piatta" di valori [R, G, B, R, G, B, ...]
        flat_palette = []

        # Interrogo la Palette per i 16 indici colore (da 0 a 15)
        for i in range(16):
            color = self.palette.get_color(i)   # Color è una tupla (R, G, B)
            flat_palette.extend(color)

        # Applico i colori all'immagine
        img.putpalette(flat_palette)

        # Salvo l'immagine sul disco
        img.save(output_path)

# Test di prova
if __name__ == '__main__':
    import sys
    # Importo i moduli necessari per il collaudo generale
    from palette import Palette
    from vram import VirtualVRAM
    from scene_parser import SceneParser
    from blitter import Blitter

    if len(sys.argv) != 6:
        print("Uso: python image_exporter.py")
        raise SystemExit(1)

    try:
        # 1. Carico tutti i dati come fatto finora
        palette = Palette(sys.argv[1])
        scene = SceneParser(sys.argv[2])
        vram = VirtualVRAM()
        vram.load_tiles(sys.argv[3])
        vram.load_sprites(sys.argv[4])

        # 2. Genero il frame buffer con il blitter
        blitter = Blitter(vram, scene)
        blitter.draw_background()
        blitter.draw_sprites()
        fb = blitter.get_frame_buffer()

        # 3. Esporto l'immagine finale
        exporter = ImageExporter(palette)
        exporter.export(fb, sys.argv[5])
        
        print(f"Immagine esportata con successo in: {sys.argv[5]}")

    except Exception as exc:
        print("Errore:", exc)
        raise