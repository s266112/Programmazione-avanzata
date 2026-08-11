# Cognome: Matessi
# Nome: Alessio
# Matricola: SM3201657

"""main.py

Entry point del programma. Implementa la classe RenderingPipeline per eseguire
la composizione completa della scena e salvare il risultato in un file PNG.
"""

import sys
from palette import Palette
from vram import VirtualVRAM
from scene_parser import SceneParser
from blitter import Blitter
from image_exporter import ImageExporter

class RenderingPipeline:
    def __init__(self, palette_path: str, scene_path: str, tiles_path: str, sprites_path: str, output_path: str) -> None:
        # Salvo i percorsi dei file di input e output
        self.palette_path = palette_path
        self.scene_path = scene_path
        self.tiles_path = tiles_path
        self.sprites_path = sprites_path
        self.output_path = output_path

    def run(self) -> None:
        # 1. Carico e valido la palette
        palette = Palette(self.palette_path)

        # 2. Carico e analizzo il file della scena
        scene = SceneParser(self.scene_path)

        # 3. Inizializzo la memoria video virtuale e carico gli asset binari
        vram = VirtualVRAM()
        vram.load_tiles(self.tiles_path)
        vram.load_sprites(self.sprites_path)

        # 4. Inizializzo il Blitter e genero il frame buffer indicizzato
        blitter = Blitter(vram, scene)
        blitter.draw_background()
        blitter.draw_sprites()
        frame_buffer = blitter.get_frame_buffer()

        # 5. Converto il frame buffer usando la palette e lo esporto su disco
        exporter = ImageExporter(palette)
        exporter.export(frame_buffer, self.output_path)


if __name__ == '__main__':
    # Mi assicuro che l'utente fornisca esattamente i 5 file richiesti
    if len(sys.argv) != 6:
        print("Uso: python main.py <palette.json> <scene.json> <tiles.bin> <sprites.bin> <output.png>")
        sys.exit(1)

    try:
        # Inizializzo la pipeline di rendering passando i parametri del terminale
        pipeline = RenderingPipeline(
            palette_path=sys.argv[1],
            scene_path=sys.argv[2],
            tiles_path=sys.argv[3],
            sprites_path=sys.argv[4],
            output_path=sys.argv[5]
        )
        
        # Avvio il processo
        pipeline.run()
        print(f"Rendering completato con successo! Immagine salvata in: {sys.argv[5]}")

    except Exception as exc:
        # Catturo eventuali errori sollevati dai moduli e li mostro a video
        print(f"Errore durante l'esecuzione: {exc}")
        sys.exit(1)