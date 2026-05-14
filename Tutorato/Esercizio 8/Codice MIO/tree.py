import random
import os
from ctypes import cdll, c_bool, c_float, c_int, pointer, POINTER, Structure

# =============================================================================
# PARTE 1: IMPLEMENTAZIONE NATIVA PYTHON
# =============================================================================

class Node:
    """
    --- CLASSE NODE ---
    Rappresenta l'unità fondamentale (il Nodo) dell'albero in Python.
    """

    def __init__(self, key, value):
        
        # 1. CARICO UTILE: Inserisco la chiave (per l'ordinamento) e il valore 
        #                  (il dato effettivo) all'interno dell'istanza.
        self.key = key       # 'key' viene usata per confrontare e posizionare il nodo.
        self.value = value   # 'value' è il dato reale memorizzato internamente.
        # ---------------------------------------------------------

        # 2. PUNTATORI LOGICI: Definisco i rami sinistro e destro. 
        #                      All'inizio sono 'None' perché il nodo nasce come una foglia isolata.
        self.left = None
        self.right = None
        # ---------------------------------------------------------

    def search(self, key):
        """
        --- SEARCH ---
        Ricerca ricorsiva di una chiave partendo dal nodo corrente.
        Ritorna il valore se trovato, altrimenti None.
        """

        # 1. CASO BASE - CHIAVE TROVATA: Se la chiave cercata corrisponde esattamente a quella
        #                                del nodo attuale, ho terminato con successo.
        if key == self.key:
            return self.value
        # ---------------------------------------------------------

        # 2. RAMO SINISTRO - (CHIAVI MINORI): Se la chiave cercata è più piccola di quella corrente, 
        #                                     deve trovarsi obbligatoriamente nel sotto-albero sinistro.
        if key < self.key:
            # Se a sinistra non c'è più nulla (None), la chiave non esiste.
            if self.left is None:
                return None
            # Altrimenti, rilancio la ricerca sul figlio sinistro.
            return self.left.search(key)
        # ---------------------------------------------------------

        # 3. RAMO DESTRO - (CHIAVI MAGGIORI): Se sono qui, significa che key > self.key.
        #                                     Controllo se esiste una strada percorribile a destra.
        elif key > self.key:
            if self.right is None:
                return None
            
            # Scendo ricorsivamente nel ramo destro.
            return self.right.search(key)
        # ---------------------------------------------------------
        
    def insert(self, key, value):
        """
        --- INSERT ---
        Inserisce una nuova coppia chiave-valore nel sotto-albero.
        Se la chiave esiste già, ne aggiorna il valore.
        """

        # 1. CHIAVE ESISTENTE - AGGIORNAMENTO: Se trovo un nodo che ha già questa chiave,
        #                                      non creo doppioni, ma aggiorno il carico utile.
        if key == self.key:
            self.value = value
        # ---------------------------------------------------------

        # 2. RAMO SINISTRO - (CHIAVI MINORI): Se la nuova chiave è minore di quella del nodo,
        #                                     deve obbligatoriamente andare a sinistra.
        elif key < self.key:
            # Se a sinistra c'è spazio vuoto, "pianto" qui il nuovo nodo.
            if self.left is None:
                self.left = Node(key, value)
            # Se il posto è occupato, passo la palla al nodo figlio (ricorsione).
            else:
                self.left.insert(key, value)
        # ---------------------------------------------------------

        # 3. RAMO DESTRO - (CHIAVI MAGGIORI): Se la chiave è maggiore, il discorso è speculare
        #                                     ma sul ramo di destra.
        elif key > self.key:
            # Se a destra è vuoto, aggancio il nuovo nodo.
            if self.right is None:
                self.right = Node(key, value)
            # Altrimenti, delego l'inserimento al figlio destro.
            else:
                self.right.insert(key, value)
        # ---------------------------------------------------------

    def __str__(self):
        """
        --- STR (STRINGA) ---
        Converto il nodo e la sua discendenza in un formato testuale.
        Uso parentesi annidate per rappresentare visivamente la gerarchia.
        """

        # 1. NODO CORRENTE: Il punto di partenza è la chiave del nodo in cui mi trovo. 
        #                   La salvo come stringa iniziale.
        s = f"{self.key}"
        # ---------------------------------------------------------

        # 2. RICORSIONE VISIVA: Se il nodo non è una foglia (ha almeno un figlio),
        #                       apro le parentesi per includere i rami.
        if self.left is not None or self.right is not None:
            # Chiamo str() sui figli innescando una reazione a catena (ricorsione).
            # Se un figlio è 'None', str(None) restituisce la stringa "None".
            s = f"({s} {str(self.left)} {str(self.right)})"
        # ---------------------------------------------------------
        
        return s


class BinarySearchTree:
    """
    --- CLASSE BINARYSEARCHTREE ---
    Agisce come gestore dell'albero. Il suo compito principale è 
    tenere traccia del punto di inizio (la radice).
    """

    def __init__(self):
        
        # 1. PUNTO DI ACCESSO: Inizializzo l'albero come vuoto. La radice non punta a nessun nodo (None).
        self.root = None
        # ---------------------------------------------------------

    def search(self, key):
        if self.root is None:
            return None
        return self.root.search(key)

    def insert(self, key, value):
        if self.root is None:
            self.root = Node(key, value)
        else:
            self.root.insert(key, value)

    def __str__(self):
        if self.root is None:
            return "Empty tree"
        return str(self.root)


# =============================================================================
# PARTE 2: INTEGRAZIONE CON IL C (Wrapping)
# =============================================================================

class CTreeNode(Structure):
    """
    Mappatura hardware della struttura '_tree_node' definita in C.
    """
    pass

CTreeNode._fields_ = [
    ("key", c_int),
    ("value", c_float),
    ("left", POINTER(CTreeNode)),
    ("right", POINTER(CTreeNode))
]

class CTree:
    """
    --- CLASSE CTREE ---
    Interfaccia Python che comanda la libreria 'libtree.so' scritta in C.
    """
    code = None

    def __init__(self):
        
        # 1. CARICAMENTO LIBRERIA: Carico i simboli binari una sola volta e configuro
        #                          le firme delle funzioni C per la corretta comunicazione.
        if CTree.code is None:
            # Cerco il percorso assoluto della libreria per evitare errori di caricamento.
            folder = os.path.dirname(os.path.abspath(__file__))
            lib_path = os.path.join(folder, "libtree.so")
            
            try:
                CTree.code = cdll.LoadLibrary(lib_path)
                CTree.code.insert.argtypes = [POINTER(CTreeNode), c_int, c_float]
                CTree.code.insert.restype = POINTER(CTreeNode)
                CTree.code.search.argtypes = [POINTER(CTreeNode), c_int, POINTER(c_float)]
                CTree.code.search.restype = c_bool
                CTree.code.destroy.argtypes = [POINTER(CTreeNode)]
            except OSError as e:
                print(f"ERRORE: Non trovo '{lib_path}'. Controlla di aver fatto 'make'.")
                raise e
        
        self.root = POINTER(CTreeNode)()
        # ---------------------------------------------------------

    def search(self, key):
        
        # 1. ESTRAZIONE DATI: In C i risultati passano spesso per puntatore.
        #                     Uso 'c_float' come buffer per ricevere il valore trovato.
        res = c_float()
        found = CTree.code.search(self.root, c_int(key), pointer(res))
        
        if found:
            return res.value
        return None
        # ---------------------------------------------------------

    def insert(self, key, value):
        self.root = CTree.code.insert(self.root, c_int(key), c_float(value))

    def __str__(self):
        """
        --- STR (C-TREE) ---
        Costruisco manualmente la stringa leggendo la memoria C tramite i puntatori.
        """
        def print_c_node(node):
            if not node:
                return "None"
            
            curr = node.contents
            s = f"{curr.key}"
            
            if curr.left or curr.right:
                s = f"({s} {print_c_node(curr.left)} {print_c_node(curr.right)})"
            return s

        if not self.root:
            return "Empty C tree"
        return print_c_node(self.root)

    def __del__(self):
        
        # 1. GESTIONE MEMORIA: Quando Python distrugge l'oggetto, 
        #                      chiamo la 'free' in C per evitare memory leak.
        if hasattr(self, 'root') and self.root:
            CTree.code.destroy(self.root)
        # ---------------------------------------------------------


# =============================================================================
# SEZIONE DI TEST (Main)
# =============================================================================

if __name__ == "__main__":
    print("--- TEST ALBERO PYTHON ---")
    bst = BinarySearchTree()
    for i in range(0, 20):
        bst.insert(random.randint(0, 30), random.random())
    
    print(bst)
    
    test_key = random.randint(0, 30)
    print(f"Searching for {test_key}: {bst.search(test_key)}")

    print("\n--- TEST ALBERO C (EXTRA) ---")
    try:
        cbst = CTree()
        for i in range(0, 20):
            cbst.insert(random.randint(0, 30), random.random())
        print(cbst)
        print(f"Searching for {test_key}: {cbst.search(test_key)}")
    except Exception as e:
        print(f"Nota: Qualcosa è andato storto nel test C: {e}")