# Cognome: Matessi
# Nome: Alessio
# Matricola: SM3201657

"""Virtual_vram.py

Decodifica i file binari packed a 4-bit di tile sheet e sprite sheet
convertendoli in matrici indicizzate numpy 256x256.
"""

from pathlib import Path
import numpy as np

class VirtualVRAM:
    # Costanti specificate dall'architettura del progetto
    DIMENSION = 256
    EXPECTED_BYTES = 32768  # (256 * 256) / 2 byte. Ogni byte ha 2 pixel

    def __init__(self) -> None:
        # Inizializzo gli sheet con None, finchè non vengono caricati
        self.tile_sheet = None
        self.sprite_sheet = None

    def _load_packed_binary(self, filepath: str) -> np.ndarray:
        # Legge e decodifica un file binario 4-bit packed in una matrice numpy
        path = Path(filepath)

        if not path.exists():
            raise FileNotFoundError(f"File binario non trovato: {filepath}")
        if not path.is_file():
            raise ValueError(f"Il percorso non è un file valido: {filepath}")

        raw_data = path.read_bytes()

        # Valido la dimensione del file
        if len(raw_data) != self.EXPECTED_BYTES:
            raise ValueError(
                f"Dimensione del file errata per {filepath}."
                f"Attesi {self.EXPECTED_BYTES} byte, trovati {len(raw_data)}."
            )

        # Converto i byte grezzi in un array 1D
        bytes_array = np.frombuffer(raw_data, dtype=np.uint8)

        # Creo un array grande il doppio per i singoli pixel (2 per ogni byte)
        pixels = np.empty(self.DIMENSION * self.DIMENSION, dtype=np.uint8)

        # Estraggo i byte 
        pixels[0::2] = bytes_array >> 4     # 4 piu alti per i pixel pari (uso shift a destra)
        pixels[1::2] = bytes_array & 0x0F   # 4 più bassi per i pixel dispari (uso maschera bit a bit) 

        # Rimodello l'array monodimensionale nella matrice 256x256
        return pixels.reshape((self.DIMENSION, self.DIMENSION))

    def load_tiles(self, filepath: str) -> None:
        # Carico e salvo la matrice del tile sheet
        self.tile_sheet = self._load_packed_binary(filepath)

    def load_sprites(self, filepath: str) -> None:
        # Carico e salvo la matrice dello sprite sheet
        self.sprite_sheet = self._load_packed_binary(filepath)

    def get_tile_sheet(self) -> np.ndarray:
        # Restituisce la matrice del tile sheet, sollevando un errore se non carica
        if self.tile_sheet is None:
            raise RuntimeError("Tile sheet non ancora caricato.")
        return self.tile_sheet

    def get_sprite_sheet(self) -> np.ndarray:
            # Restituisce la matrice dello sprite sheet, sollevando un errore se non carica
            if self.sprite_sheet is None:
                raise RuntimeError("Sprite sheet non ancora caricato.")
            return self.sprite_sheet


# Test di prova
if __name__ == '__main__':
    import sys
    
    # Questo controllo mi assicura di passare i file quando lancio lo script da terminale
    if len(sys.argv) != 3:
        print("Uso: python vram.py <tiles.bin> <sprites.bin>")
        raise SystemExit(1)
        
    try:
        vram = VirtualVRAM()
        vram.load_tiles(sys.argv[1])
        vram.load_sprites(sys.argv[2])
        print("VirtualVRAM caricata con successo!")
        print("Shape Tile Sheet:", vram.get_tile_sheet().shape, "- Dtype:", vram.get_tile_sheet().dtype)
        print("Shape Sprite Sheet:", vram.get_sprite_sheet().shape, "- Dtype:", vram.get_sprite_sheet().dtype)
    except Exception as exc:
        print("Errore:", exc)
        raise