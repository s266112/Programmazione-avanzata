# Cognome: Matessi
# Nome: Alessio
# Matricola: SM3201657

"""
palette.py
Implementazione della classe `Palette` per il progetto di renderer 2D.
Durante lo sviluppo manteniamo commenti didattici in italiano.
"""

import json
from typing import List, Tuple
import numpy as np


class Palette:
    """
    - Carica e valida una palette di 16 colori da file JSON

    - Ogni colore nel JSON deve essere una lista/tupla di 3 interi (r, g, b) in [0, 255].
      Internamente i colori sono salvati come array numpy dtype uint8 per efficenza.
    """

    def __init__(self, filepath: str):

        # Apro il file in modo robusto e catturo errori di IO/JSON
        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                data = json.load(f)
        except FileNotFoundError:
            raise FileNotFoundError(f"File non trovato: {filepath}")
        except json.JSONDecodeError as e:
            raise ValueError(f"Errore nel parsing JSON di {filepath}: {e}")

        # Verifico che il JSON rappresenti una lista
        if not isinstance(data, list):
            raise TypeError("La palette deve essere rappresentata come una lista JSON.")

        # Verifico la lunghezza esatta di 16 colori
        if len(data) != 16:
            raise ValueError(f"Errore: la palette deve essere esattamente 16 colori; trovati {len(data)}.")

        # Valido ogni voce: deve essere (r, g, b) con interi 0..255
        validated: List[Tuple[int, int, int]] = []
        for i, entry in enumerate(data):
            if not (isinstance(entry, (list, tuple)) and len(entry) == 3):
                raise ValueError(f"Elemento alla posizione {i} non valido: deve essere lista/tupla di 3 elementi RGB.")
            r, g, b = entry
            for name, ch in (("r", r), ("g", g), ("b", b)):
                if not isinstance(ch, int):
                    raise TypeError(f"Componente {name} del colore {i} non è un intero: {ch!r}")
                if not (0 <= ch <= 255):
                    raise ValueError(f"Componente {name} del colore {i} fuori range 0-255: {ch}")
            validated.append((r, g, b))

        # Conservo i colori come array numpy dtype uint8 (shape (16,3))
        self._colors = np.array(validated, dtype=np.uint8)

    def get_color(self, index: int) -> Tuple[int, int, int]:
        """"
        - Restituisce il colore (r,g,b) per un indice 0..15.

        - Controlla tipo e range per fornire errori chiari.
        """
        if not isinstance(index, int):
            raise TypeError("L'indice deve essere un intero.")
        if not (0 <= index < 16):
            raise IndexError(f"Indice fuori range: {index} (atteso 0..15).")
        return tuple(self._colors[index].tolist())

    def as_numpy(self) -> np.ndarray:
        """
        - Ritorna una copia dell'array numpy della palette (shape (16,3)).
          Uso copy() per evitare che il chiamante muti lo stato interno.

        """
        return self._colors.copy()

    def to_pillow_palette(self) -> List[int]:
        """
        - Restituisce una lista di 256*3 interi compatibile con Image.putpalette.
          I primi 16 colori sono quelli della palette; il resto è padding con zeri.
        """
        flat: List[int] = []
        for r, g, b in self._colors.tolist():
            flat.extend([int(r), int(g), int(b)])
        pad_len = 256 * 3 - len(flat)
        if pad_len > 0:
            flat.extend([0] * pad_len)
        return flat

    def __repr__(self) -> str:
        return f"Palette(16 colori, dtype={self._colors.dtype})"

 # Test rapido usando gli esempi forniti nella cartella 'Esempio progetti'
if __name__ == "__main__":
    import os
    example = os.path.join("Esempio progetti", "palette.json")
    try:
        p = Palette(example)
        print("Palette caricata correttamente.")
        print("Colore indice 1:", p.get_color(1))
        print("Array numpy shape:", p.as_numpy().shape, "dtype:", p.as_numpy().dtype)
        print("Lunghezza palette Pillow:", len(p.to_pillow_palette()))
    except Exception as exc:
        print("Errore durante il test:", exc)
    
                    
                
            