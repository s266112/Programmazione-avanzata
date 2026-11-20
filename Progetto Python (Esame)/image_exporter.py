# Cognome: Matessi
# Nome: Alessio
# Matricola: SM3201657

"""image_exporter.py

Converte il frame buffer indicizzato in un'immagine RGB usando la palette
e la salva come PNG (Pillow è usata solo per il salvataggio).
"""

from PIL import Image
import numpy as np

class ImageExporter:
    def __init__(self, palette) -> None:
        # Salvo il riferimento alla palette per poter applicare i colori
        self.palette = palette

    def to_rgb(self, frame_buffer: np.ndarray) -> np.ndarray:
        # Converto il frame buffer indicizzato in RGB.
        # Input: matrice 480x640 di indici (0..15). Output: matrice 480x640x3 np.uint8 di colori RGB
        if frame_buffer.ndim != 2:
            raise ValueError("Il frame buffer deve essere una matrice 2D.")
        if frame_buffer.max() >= 16:
            raise ValueError("Il frame buffer contiene indici fuori dalla palette (0..15).")

        # Uso il frame buffer come indice nell'array della palette (16x3):
        # ogni pixel viene sostituito dalla terna RGB corrispondente
        colors = self.palette.as_numpy()
        return colors[frame_buffer]

    def export(self, frame_buffer: np.ndarray, output_path: str) -> None:
        # Converto il frame buffer in RGB e lo salvo come PNG.
        # Input: frame buffer indicizzato e percorso di output. Output: nessuno, scrive il file
        rgb = self.to_rgb(frame_buffer)

        # Pillow serve solo per codificare l'array RGB in PNG
        img = Image.fromarray(rgb, mode='RGB')
        img.save(output_path, format='PNG')

# Test di prova
if __name__ == '__main__':
    import sys
    # Importo i moduli necessari per il collaudo generale
    from palette import Palette
    from vram import VirtualVRAM
    from scene_parser import SceneParser
    from blitter import Blitter

    if len(sys.argv) != 6:
        print("Uso: python image_exporter.py <palette.json> <scene.json> <tiles.bin> <sprites.bin> <output.png>")
        raise SystemExit(1)

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