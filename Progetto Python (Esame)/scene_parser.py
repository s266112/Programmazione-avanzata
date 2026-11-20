# Cognome: Matessi
# Nome: Alessio
# Matricola: SM3201657

"""scene_parser.py

Legge il file JSON della scena e ne estrae la mappa dei tile,
l'indice di trasparenza e la lista degli sprite.
"""

import json
from pathlib import Path
from typing import List, Dict, Any
import numpy as np


def _is_int(value: Any) -> bool:
    # Vero se è un intero (escludo i bool, che in Python sono int)
    return isinstance(value, int) and not isinstance(value, bool)


class SceneParser:
    # Dimensioni fisse della mappa del fondale
    MAP_ROWS = 15
    MAP_COLS = 20

    # Valori ammessi dal progetto
    PALETTE_SIZE = 16
    TILE_COUNT = 64
    SPRITE_COUNT = 16
    VALID_ROTATIONS = (0, 90, 180, 270)
    SPRITE_FIELDS = ("id", "x", "y", "flip_h", "flip_v", "rotation")

    def __init__(self, filepath: str) -> None:
        # Attributi interni della scena
        self._transparent_index: int = 0
        self._tile_map: np.ndarray = np.zeros((self.MAP_ROWS, self.MAP_COLS), dtype=np.uint8)
        self._sprites: List[Dict[str, Any]] = []

        # Carico e valido immediatamente la scena
        self._load_scene(filepath)

    def _load_scene(self, filepath: str) -> None:
        # Carico e valido la scena. Input: percorso del JSON. Output: nessuno, salvo i dati negli attributi
        path = Path(filepath)

        # Controllo l'esistenza del file
        if not path.is_file():
            raise FileNotFoundError(f"File di scena non trovato o non valido: {filepath}")

        # Parsing sicuro del JSON
        try:
            data = json.loads(path.read_text(encoding='utf-8'))
        except json.JSONDecodeError as exc:
            raise ValueError(f"Errore nel parsing JSON della scena {filepath}: {exc}") from exc

        if not isinstance(data, dict):
            raise TypeError("Il file di scena deve contenere un oggetto JSON.")

        # Validazione strutturale: mi assicuro che le tre chiavi principali esistano
        for key in ("transparent_index", "tile_map", "sprites"):
            if key not in data:
                raise KeyError(f"Chiave mancante nel file di scena: '{key}'")

        # 1. Valido e salvo "transparent_index" (indice trasparenza)
        self._transparent_index = self._validate_transparent_index(data["transparent_index"])

        # 2. Valido e salvo "tile_map" (mappa dei tile)
        self._tile_map = self._validate_tile_map(data["tile_map"])

        # 3. Valido e salvo "sprites" (lista degli sprite)
        raw_sprites = data["sprites"]
        if not isinstance(raw_sprites, list):
            raise TypeError("La voce 'sprites' deve essere una lista JSON.")
        self._sprites = [self._validate_sprite(s, i) for i, s in enumerate(raw_sprites)]

    def _validate_transparent_index(self, value: Any) -> int:
        # Controllo l'indice di trasparenza. Input: valore dal JSON. Output: indice 0..15
        if not _is_int(value):
            raise TypeError(f"'transparent_index' deve essere un intero, trovato {value!r}.")
        if not (0 <= value < self.PALETTE_SIZE):
            raise ValueError(f"'transparent_index' deve essere tra 0 e 15, trovato {value}.")
        return value

    def _validate_tile_map(self, raw_map: Any) -> np.ndarray:
        # Controllo la tile map. Input: lista di liste dal JSON. Output: matrice numpy 15x20 uint8
        if not isinstance(raw_map, list) or len(raw_map) != self.MAP_ROWS:
            raise ValueError(f"La tile_map deve contenere esattamente {self.MAP_ROWS} righe.")

        for r, row in enumerate(raw_map):
            if not isinstance(row, list) or len(row) != self.MAP_COLS:
                raise ValueError(f"Errore nella riga {r} della tile_map: deve avere {self.MAP_COLS} colonne.")

            for c, tile_id in enumerate(row):
                if not _is_int(tile_id):
                    raise TypeError(f"Tile in ({r}, {c}) non è un intero: {tile_id!r}.")
                if not (0 <= tile_id < self.TILE_COUNT):
                    raise ValueError(f"Tile in ({r}, {c}) fuori range: {tile_id} (atteso 0..63).")

        # Converto la mappa in un array numpy per una gestione rapida durante il rendering
        return np.array(raw_map, dtype=np.uint8)

    def _validate_sprite(self, sprite: Any, index: int) -> Dict[str, Any]:
        # Controllo uno sprite. Input: sprite dal JSON e sua posizione. Output: dizionario validato
        if not isinstance(sprite, dict):
            raise TypeError(f"Lo sprite {index} deve essere un oggetto JSON.")

        for field in self.SPRITE_FIELDS:
            if field not in sprite:
                raise KeyError(f"Campo '{field}' mancante nello sprite {index}.")

        if not _is_int(sprite["id"]):
            raise TypeError(f"Sprite {index}: 'id' deve essere un intero.")
        if not (0 <= sprite["id"] < self.SPRITE_COUNT):
            raise ValueError(f"Sprite {index}: 'id' fuori range: {sprite['id']} (atteso 0..15).")

        # x e y possono essere anche negativi (il clipping lo fa il Blitter)
        for coord in ("x", "y"):
            if not _is_int(sprite[coord]):
                raise TypeError(f"Sprite {index}: '{coord}' deve essere un intero.")

        for flag in ("flip_h", "flip_v"):
            if not isinstance(sprite[flag], bool):
                raise TypeError(f"Sprite {index}: '{flag}' deve essere true o false.")

        if not _is_int(sprite["rotation"]) or sprite["rotation"] not in self.VALID_ROTATIONS:
            raise ValueError(f"Sprite {index}: 'rotation' non valida: {sprite['rotation']!r} (ammessi 0, 90, 180, 270).")

        return {field: sprite[field] for field in self.SPRITE_FIELDS}

    # Getter (Metodi di accesso protetto)
    def get_transparent_index(self) -> int:
        return self._transparent_index

    def get_tile_map(self) -> np.ndarray:
        return self._tile_map.copy() # Restituiamo una copia per proteggere il dato originale

    def get_sprites(self) -> List[Dict[str, Any]]:
        return [dict(s) for s in self._sprites]


# Test di prova
if __name__ == '__main__':
    import sys

    # Mi assicuro che venga passato il percorso del JSON da terminale
    if len(sys.argv) != 2:
        print("Uso: python scene_parser.py <scene.json>")
        raise SystemExit(1)

    parser = SceneParser(sys.argv[1])
    print("Scena caricata con successo!")
    print(f"Indice Trasparenza: {parser.get_transparent_index()}")
    print(f"Shape Tile Map: {parser.get_tile_map().shape}")
    print(f"Numero di Sprites Trovati: {len(parser.get_sprites())}")

    # Stampo il primo sprite per verificare i dati
    if len(parser.get_sprites()) > 0:
        primo_sprite = parser.get_sprites()[0]
        print(f"Dati primo sprite: ID={primo_sprite.get('id')}, X={primo_sprite.get('x')}, Y={primo_sprite.get('y')}")