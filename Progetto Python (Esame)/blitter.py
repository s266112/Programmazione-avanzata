# Cognome: Matessi
# Nome: Alessio
# Matricola: SM3201657

"""blitter.py

Si occupa di estrarre tile e sprite, applicare trasformazioni
e incollarli sul frame buffer indicizzato.
"""

import numpy as np

class Blitter:
    # Costanti specificate dal progetto
    SCREEN_WIDTH = 640
    SCREEN_HEIGHT = 480
    TILE_SIZE = 32
    SHEET_COLS = 8              # Colonne per il tile sheet (fondale)
    SPRITE_SIZE = 64            # Dimensione in pixel degli sprite
    SPRITE_COLS = 4             # Colonne per lo sprite sheet (griglia 4x4)

    def __init__(self, vram, scene) -> None:
        # Inizializzo il Blitter con le dipendenze necessarie e preparo lo schermo
        self.vram = vram
        self.scene = scene

        # Frame buffer: conterrà solo gli INDICI di palette (0-15), non i colori veri
        self.frame_buffer = np.zeros((self.SCREEN_HEIGHT, self.SCREEN_WIDTH), dtype=np.uint8)

    def draw_background(self) -> None:
        # Scorre la tile_map della scena e disegna ogni tile sul frame buffer
        tile_map = self.scene.get_tile_map()
        tile_sheet = self.vram.get_tile_sheet()

        # Itero sulle 15 righe e 20 colonne della mappa
        rows, cols = tile_map.shape
        for row in range(rows):
            for col in range(cols):
                tile_id = tile_map[row, col]

                # Calcolo le coordinate sorgente sul tile sheet (griglia 8x8)
                sheet_row = (tile_id // self.SHEET_COLS) * self.TILE_SIZE
                sheet_col = (tile_id % self.SHEET_COLS) * self.TILE_SIZE

                # Estraggo l'intero blocco 32x32 in un colpo solo
                tile_pixels = tile_sheet[
                    sheet_row : sheet_row + self.TILE_SIZE,
                    sheet_col : sheet_col + self.TILE_SIZE
                ]

                # Calcolo le coordinate di destinazione sul frame buffer
                fb_row = row * self.TILE_SIZE
                fb_col = col * self.TILE_SIZE

                # Copio il blocco sul frame buffer virtuale
                self.frame_buffer[
                    fb_row : fb_row + self.TILE_SIZE,
                    fb_col : fb_col + self.TILE_SIZE 
                ] = tile_pixels

    def draw_sprites(self) -> None:
        # Recupero i dati necessari dalla scena e dalla VRAM
        sprites = self.scene.get_sprites()
        sprite_sheet = self.vram.get_sprite_sheet()
        transparent_idx = self.scene.get_transparent_index()

        for sprite in sprites:
            sprite_id = sprite["id"]
            x = sprite["x"]
            y = sprite["y"]
            flip_h = sprite.get("flip_h", False)
            flip_v = sprite.get("flip_v", False)
            rotation = sprite.get("rotation", 0)

            # Calcolo le coordinate sorgente sullo sprite sheet (griglia 4x4)
            sheet_row = (sprite_id // self.SPRITE_COLS) * self.SPRITE_SIZE
            sheet_col = (sprite_id % self.SPRITE_COLS) * self.SPRITE_SIZE

            # Estraggo il blocco 64x64 iniziale
            sprite_pixels = sprite_sheet[
                sheet_row : sheet_row + self.SPRITE_SIZE,
                sheet_col : sheet_col + self.SPRITE_SIZE
            ].copy()

            # Applico il ribaltamento orizzontale
            if flip_h:
                sprite_pixels = np.fliplr(sprite_pixels)

            # Applico il ribaltamento verticale
            if flip_v:
                sprite_pixels = np.flipud(sprite_pixels)

            # Applico la rotazione (espressa in gradi: 90, 180, 270)
            if rotation > 0:
                k = rotation // 90
                sprite_pixels = np.rot90(sprite_pixels, -k)

            # Clipping geometrico: calcolo le coordinate reali considerando i bordi dello schermo
            x_start = max(0, x)
            y_start = max(0, y)
            x_end = min(self.SCREEN_WIDTH, x + self.SPRITE_SIZE)
            y_end = min(self.SCREEN_HEIGHT, y + self.SPRITE_SIZE)

            # Se lo sprite è completamente fuori dallo schermo, lo salto
            if x_start >= x_end or y_start >= y_end:
                continue

            # Calcolo gli offset per ritagliare lo sprite se esce dai bordi
            sprite_x_start = x_start - x
            sprite_y_start = y_start - y
            sprite_x_end = sprite_x_start + (x_end - x_start)
            sprite_y_end = sprite_y_start + (y_end - y_start)

            # Estraggo le porzioni corrispondenti di schermo e sprite
            fb_slice = self.frame_buffer[y_start:y_end, x_start:x_end]
            sprite_slice = sprite_pixels[sprite_y_start:sprite_y_end, sprite_x_start:sprite_x_end]

            # Creo la maschera sulle dimensioni corrette e incollo
            mask = sprite_slice != transparent_idx
            fb_slice[mask] = sprite_slice[mask]


    def get_frame_buffer(self) -> np.ndarray:
        return self.frame_buffer.copy()


# Test di prova
if __name__ == '__main__':
    import sys
    # Importo le classi precedenti per il collaudo integrato
    from vram import VirtualVRAM
    from scene_parser import SceneParser

    if len(sys.argv) != 4:
        print("Uso: python blitter.py <scene.json> <tiles.bin> <sprites.bin>")
        raise SystemExit(1)

    try:
        # 1. Preparo i dati
        scene = SceneParser(sys.argv[1])
        vram = VirtualVRAM()
        vram.load_tiles(sys.argv[2])
        vram.load_sprites(sys.argv[3])

        # 2. Inizializzo il blitter e disegno il fondale
        blitter = Blitter(vram, scene)
        blitter.draw_background()
        blitter.draw_sprites()
        
        fb = blitter.get_frame_buffer()
        print("Fondale e sprites disegnati con successo!")
        print(f"Frame Buffer Shape: {fb.shape} (Atteso: 480, 640)")
        
        # Testo un pixel specifico
        pixel_test = fb[420, 10]
        print(f"Valore indicizzato di test (Y=420, X=10): {pixel_test}")

    except Exception as exc:
        print("Errore:", exc)
        raise