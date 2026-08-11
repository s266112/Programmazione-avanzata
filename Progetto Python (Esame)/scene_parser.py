# Cognome: Matessi
# Nome: Alessio
# Matricola: SM3201657

"""scene_parser.py

Implementa la classe SceneParser per il renderer 2D.
Legge il file JSON della scena e ne estrae la mappa dei tile,
l'indice di trasparenza e la lista degli sprite.
"""

import json
from pathlib import Path
from typing import List, Dict, Any
import numpy as np

class SceneParser:
    # Dimensioni fisse della mappa del fondale
    MAP_ROWS = 15
    MAP_COLS = 20

    def __init__(self, filepath: str) -> None:
        # Attributi interni della scena
        self._transparent_index: int = 0
        self._tile_map: np.ndarray = None
        self._sprites: List[Dict[str, Any]] = []

        # Carico e valido immediatamente la scena
        self._load_scene(filepath)

    def _load_scene(self, filepath: str) -> None:
        path = Path(filepath)

        # Controllo l'esistenza del file
        if not path.exists() or not path.is_file():
            raise FileNotFoundError(f"File di scena non trovato o non valido: {filepath}")

        # Parsing sicuro del JSON
        try:
            content = path.read_text(encoding='utf-8')
            data = json.loads(content)
        except json.JSONDecodeError as exc:
            raise ValueError(f"Errore nel parsing JSON della scena {filepath}: {exc}")

        # Validazione strutturale: mi assicuro che le tre chiavi principali esistano
        for key in ["transparent_index", "tile_map", "sprites"]:
            if key not in data:
                raise KeyError(f"Chiave mancante nel file di scena: '{key}'")

        # 1. Valido e salvo "transparent_index" (indice trasparenza)
        self._transparent_index = data["transparent_index"]
        if not isinstance(self._transparent_index, int) or not (0 <= self._transparent_index <= 15):
            raise ValueError("L'indice di trasparenza deve essere un intero tra 0 e 15.")

        # 2. Valido e salvo "tile_map" (mappa dei tile)
        raw_map = data["tile_map"]
        if not isinstance(raw_map, list) or len(raw_map) != self.MAP_ROWS:
            raise ValueError(f"La tile_map deve contenere esattamente {self.MAP_ROWS} righe")

        for i, row in enumerate(raw_map):
            if not isinstance(row, list) or len(row) != self.MAP_COLS:
                raise ValueError(f"Errore nella riga {i} della tile_map: deve avere {self.MAP_COLS} colonne.")

        # Converto la mappa in un array numpy per una gestione rapida durante il rendering
        self._tile_map = np.array(raw_map, dtype=np.uint8)

        # 3. Valido e salvo "raw_sprite" (lista degli sprite)
        raw_sprites = data["sprites"]
        if not isinstance(raw_sprites, list):
            raise TypeError("La voce 'sprites' deve essere una lista JSON.")
        self._sprites = raw_sprites

    # GETTER (Metodi di accesso protetto)
    def get_transparent_index(self) -> int:
        return self._transparent_index

    def get_tile_map(self) -> np.ndarray:
        return self._tile_map.copy() # Restituiamo una copia per proteggere il dato originale

    def get_sprites(self) -> List[Dict[str, Any]]:
        return self._sprites.copy()


# TEST DI MODULO 
if __name__ == '__main__':
    import sys
    
    # Assicuriamoci che venga passato il percorso del JSON da terminale
    if len(sys.argv) != 2:
        print("Uso: python scene_parser.py ")
        raise SystemExit(1)
        
    try:
        parser = SceneParser(sys.argv[1])
        print("Scena caricata con successo!")
        print(f"Indice Trasparenza: {parser.get_transparent_index()}")
        print(f"Shape Tile Map: {parser.get_tile_map().shape}")
        print(f"Numero di Sprites Trovati: {len(parser.get_sprites())}")
        
        # Stampiamo il primo sprite per verificare i dati
        if len(parser.get_sprites()) > 0:
            primo_sprite = parser.get_sprites()[0]
            print(f"Dati primo sprite: ID={primo_sprite.get('id')}, X={primo_sprite.get('x')}, Y={primo_sprite.get('y')}")
            
    except Exception as exc:
        print("Errore:", exc)
        raise





        