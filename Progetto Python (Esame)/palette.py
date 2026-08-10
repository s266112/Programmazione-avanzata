# Cognome: Matessi
# Nome: Alessio
# Matricola: SM3201657

"""palette.py

Implementa la classe Palette per il renderer 2D.
Legge una palette JSON con 16 colori RGB e la converte in un array numpy.
"""

import json
from pathlib import Path
from typing import List, Tuple

import numpy as np

class Palette:      # Classe che rappresenta una palette indicizzata a 16 colori.

    COLOR_COUNT = 16
    MIN_CHANNEL = 0
    MAX_CHANNEL = 255

    def __init__(self, filepath: str) -> None:      
        # Inizializzo la palette leggendo e validando il file JSON
        self._colors = self._load_palette(filepath)

    def _load_palette(self, filepath: str) -> np.ndarray:
        # Carico il file JSON e converto i colori in un array numpy uint8
        path = Path(filepath)
        if not path.exists():
            raise FileNotFoundError(f"File non trovato: {filepath}")
        if not path.is_file ():
            raise ValueError(f"Il percorso non è un file: {filepath}")

        try:
            content = path.read_text(encoding='utf-8')
            data = json.loads(content)
        except json.JSONDecodeError as exc:
            raise ValueError(f"Errore nel parsing JSON di {filepath}: {exc}")

        if not isinstance(data, list):
            raise TypeError("La palette deve essere una lista JSON.")
        if len(data) != self.COLOR_COUNT:
            raise ValueError(f"La palette deve contenere esattamente {self.COLOR_COUNT} colori; trovati {len(data)}.")

        validated_colors: List[tuple[int, int, int]] = []
        for index, entry in enumerate(data):
            validated_colors.append(self._validate_color_entry(entry, index))

        return np.array(validated_colors, dtype=np.uint8)

    def _validate_color_entry(self, entry: object, index: int) -> Tuple[int, int, int]:
        # Controlla che l'elemento sia una lista RGB valida di tre interi.
        if not isinstance(entry, list) or len(entry) != 3:
            raise ValueError(f"Colore {index} non valido: deve essere una lista di 3 interi RGB.")

        r, g, b = entry
        for channel_name, channel_value in (("r", r), ("g", g), ("b", b)):
            if not isinstance(channel_value, int):
                raise TypeError(
                    f"Componente {channel_name} del colore {index} non è un intero: {channel_value!r}"
                )
            if not (self.MIN_CHANNEL <= channel_value <= self.MAX_CHANNEL):
                raise ValueError(
                    f"Componente {channel_name} del colore {index} fuori range: {channel_value}. "
                    f"Valori validi: {self.MIN_CHANNEL}..{self.MAX_CHANNEL}."
                )

        return r, g, b

    def get_color(self, index: int) -> Tuple[int, int, int]:
        # Restituisce il colore RGB corrispondente all'indice 0..15.
        if not isinstance(index, int):
            raise TypeError("L'indice deve essere un intero.")
        if not (0 <= index < self.COLOR_COUNT):
            raise IndexError(
                f"Indice fuori range: {index} (atteso 0..{self.COLOR_COUNT - 1})."
            )
        return tuple(self._colors[index].tolist())

    def as_numpy(self) -> np.ndarray:
        # Ritorna una copia dell'array numpy dei colori
        return self._colors.copy()

    def to_pillow_palette(self) -> List[int]:
        # Restituisce una lista di 768 interi compatibile con Pillow Image.putpalette()
        flat_palette: List[int] = []
        for r, g, b in self._colors.tolist():
            flat_palette.extend([int(r), int(g), int(b)])

        padding = 256 * 3 - len(flat_palette)
        if padding > 0:
            flat_palette.extend([0] * padding)
        return flat_palette

# Test di prova
if __name__ == '__main__':
    import sys

    if len(sys.argv) != 2:
        print('Uso: python palette.py <palette.json>')
        raise SystemExit(1)

    try:
        palette = Palette(sys.argv[1])
        print('Palette caricata correttamente.')
        print('Colore indice 0:', palette.get_color(0))
        print('Numero colori:', palette.as_numpy().shape[0])
        print('Lunghezza palette Pillow:', len(palette.to_pillow_palette()))
    except Exception as exc:
        print('Errore:', exc)
        raise