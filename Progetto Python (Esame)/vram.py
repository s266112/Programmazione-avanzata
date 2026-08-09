# Cognome: Matessi
# Nome: Alessio
# Matricola: SM3201657

"""
Questa classe legge i file binari di tile sheet e sprite sheet e li decodifica 
in matrici di indici palette usando numpy.
"""

from pathlib import Path
from typing import Optional

import numpy as np

class VirtualVRAM:
    """
    VirtualVRAM gestisce il tile sheet e lo sprite sheet in formato binario packed.

    - Il tile sheet è un'immagine 256x256 di pixel indicizzati a 4 bit.
    - Lo sprite sheet è un'immagine 256x256 di pixel indicizzati a 4 bit.
    - Ogni byte contiene due pixel: nibble alto = primo pixel, nibble basso = secondo.
    - I file binari devono essere esattamente 32768 byte.
    """

    DIMENSION = 256
    PACKED_FILE_SIZE = DIMENSION * DIMENSION // 2

    def __init__(self) -> None:
        # Stato interno: non sono ancora caricati i fogli tile e sprite
        self._tile_sheet: Optional[np.ndarray] = None
        self._sprite_sheet: Optional[np.ndarray] = None

    @staticmethod
    def _read_packed_file(filepath: str) -> bytes:
        """
        Legge un file binario e verifica la dimensione corretta
        per una matrice 256x256 di pixel 4-bit packed.
        """
        path = Path(filepath)

        # Controllo che il file esista
        if not path.exists():
            raise FileNotFoundError(f"File non trovato: {filepath}")

        # Controllo che il percorso sia effettivamente un file
        if not path.is_file():
            raise ValueError(f"Percorso non valido per il file binario: {filepath}")

        raw = path.read_bytes()

        # La dimensione deve essere esattamente 32768 byte
        if len(raw) != VirtualVRAM.PACKED_FILE_SIZE:
            raise ValueError(
                f"Dimensione file non valida: {filepath} ha {len(raw)} byte, "
                f"attesi {VirtualVRAM.PACKED_FILE_SIZE}."
            )
        return raw

    @staticmethod
    def _decode_nibbles(raw: bytes) -> np.ndarray:
        """
        Decodifica i byte packed in una matrice 256x256 di indici palette.
        Ogni byte produce due pixel: nibble alto e nibble basso.
        """
        expected_bytes = VirtualVRAM.PACKED_FILE_SIZE
        if len(raw) != expected_bytes:
            raise ValueError(f"Numero di byte errato per la decodifica: {len(raw)} invece di{expected_bytes}.")

        total_pixels = VirtualVRAM.DIMENSION * VirtualVRAM.DIMENSION

        # Alloco l'array dei pixel come uint8
        pixels = np.empty(total_pixels, dtype=np.uint8)

        # Decodifico ogni byte in due indici palette
        for i, byte in enumerate(raw):
            high = (byte >> 4) & 0x0F
            low = byte & 0x0F
            pixels[2 * i] = high
            pixels[2 * i + 1] = low

        # Rimodello in una matrice 256x256
        return pixels.reshape((VirtualVRAM.DIMENSION, VirtualVRAM.DIMENSION))

    def load_tile_sheet(self, filepath: str) -> None:
        """
        Carica e decodifica il tile sheet dal file binario.
        """
        raw = self._read_packed_file(filepath)
        self._tile_sheet = self._decode_nibbles(raw)

    def load_sprite_sheet(self, filepath: str) -> None:
        """
        Carica e decodifica lo sprite sheet dal file binario.
        """
        raw = self._read_packed_file(filepath)
        self._sprite_sheet = self._decode_nibbles(raw)

    @property
    def tile_sheet(self) -> np.ndarray:
        """
        Restituisce la matrice del tile sheet caricata.
        Solleva RuntimeError se il tile sheet non è ancora stato caricato.
        """
        if self._tile_sheet is None:
            raise RuntimeError("Tile sheet non caricato")
        return self._tile_sheet

    @property
    def sprite_sheet(self) -> np.ndarray:
        """
        Restituisce la matrice dello sprite sheet caricata.
        Solleva RuntimeError se lo sprite sheet non è ancora stato caricato.
        """
        if self._sprite_sheet is None:
            raise RuntimeError("Sprite sheet non caricato")
        return self._sprite_sheet

    def __repr__(self) -> str:
        return (
            f"VirtualVRAM(tile_sheet={'loaded' if self._tile_sheet is not None else 'empty'}, "
            f"sprite_sheet={'loaded' if self._sprite_sheet is not None else 'empty'})"
        )

# Test rapido usando gli esempi forniti nella cartella 'Esempio progetti'

if __name__ == "__main__":
    import os

    example_folder = os.path.join("Esempio progetti")
    tile_path = os.path.join(example_folder, "tiles.bin")
    sprite_path = os.path.join(example_folder, "sprites.bin")

    try:
        vram = VirtualVRAM()
        vram.load_tile_sheet(tile_path)
        vram.load_sprite_sheet(sprite_path)
        print("Tile sheet caricato:", vram.tile_sheet.shape, vram.tile_sheet.dtype)
        print("Sprite sheet caricato:", vram.sprite_sheet.shape, vram.sprite_sheet.dtype)
        print("Esempio pixel [0,0] tile:", int(vram.tile_sheet[0, 0]))
        print("Esempio pixel [0,1] sprite:", int(vram.sprite_sheet[0, 1]))
    except Exception as exc:
        print("Errore durante il test VirtualVRAM:", exc)


    

