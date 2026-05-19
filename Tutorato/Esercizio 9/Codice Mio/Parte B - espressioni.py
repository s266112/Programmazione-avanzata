# ================================
# ESPRESSIONI - PARTE B
# ================================

# -- COSA FA QUESTO PROGRAMMA? : Costruisce e valuta espressioni matematiche scritte in 
#                             NOTAZIONE POLACCA INVERSA (RPN - Reverse Polish Notation).


# -- CHE COS'È LA NOTAZIONE POLACCA INVERSA (RPN)? È un modo di scrivere le espressioni matematiche SENZA parentesi,
#                                                  mettendo PRIMA gli operandi (numeri/variabili) e DOPO l'operatore.


# Esempio:
#   Notazione normale (infissa)    →    Notazione polacca inversa (RPN)
#   -----------------------------------------------------------------
#   2 + 3                          →    2 3 +
#   (2 + 3) * x                    →    2 3 + x *
#   (2 + 3) * (x - 1)              →    2 3 + x 1 - *
#   2 + 3 * x                      →    2 3 x * +
#   1 / x                          →    x 1/          (operatore unario)
#   |x|                            →    x abs         (operatore unario)
#   (2 + 3) ** 2                   →    2 3 + 2 **



# -- COME SI CALCOLA UNA ESPRESSIONE IN RPN?: Si usa una PILA (stack):

#   1. Leggi un pezzo (numero, variabile o operatore)
#   2. Se è un numero o una variabile → lo metti sulla pila
#   3. Se è un operatore:
#      - Prelevi (pop) dalla pila gli argomenti che servono
#      - (2 per +, -, *, /, **, %; 1 per 1/, abs)
#      - Esegui l'operazione
#      - Metti il risultato sulla pila
#   4. Alla fine, sulla pila rimane UN solo elemento → il risultato



# Esempio passo-passo con "2 3 + x *" (con x = 5):
#   Passo 1: leggi "2"  → pila = [2]
#   Passo 2: leggi "3"  → pila = [2, 3]
#   Passo 3: leggi "+"  → prendi 3 e 2, fai 2+3=5 → pila = [5]
#   Passo 4: leggi "x"  → pila = [5, 5]  (x vale 5)
#   Passo 5: leggi "*"  → prendi 5 e 5, fai 5*5=25 → pila = [25]

#   RISULTATO: 25


# VANTAGGI DELL'RPN:
# - NIENTE PARENTESI: L'ordine delle operazioni è implicito nell'ordine
# - NIENTE PRECEDENZE: Non serve ricordare che * viene prima di +
# - FACILE DA CALCOLARE CON UN COMPUTER: Basta una pila, niente parsing complesso



class PilaVuotaException(Exception):
    """
    - Eccezione sollevata quando si tenta di fare pop da una pila vuota.
    """
    pass


class Pila:
    """
    - CLASSE PILA: Implementazione di una pila (LIFO) usando una lista Python
    """

    def __init__(self):
        """
        - INIT: Inizializza una pila vuota.
        """
        self.dati = []


    def push(self, x):
        """
        - PUSH: Inserisce un elemento in cima alla pila.
        """
        self.dati.append(x)


    def pop(self):
        """
        - POP: Rimuove e restituisce l'elemento in cima alla pila.
        
        # Perché sollevo PilaVuotaException?
        - La consegna lo richiede.
        - Distingue tra pila vuota e altri errori.
        """
        if self.dati == []:
            raise PilaVuotaException
        risultato = self.dati[-1]
        self.dati = self.dati[0:-1]
        return risultato


    def __str__(self):
        """
        - STR: Restituisce una rappresentazione testuale della pila.
        """
        return " ".join([str(s) for s in self.dati])


# ============================================================================
# CLASSI BASE DEL SISTEMA DI ESPRESSIONI
# ============================================================================

class VariabileMancanteException(Exception):
    """
    - Eccezione sollevata quando una variabile non è definita nell'ambiente.
    
    # Perché una eccezione separata?
    - Permette al chiamante di distinguere tra errore di sintassi e variabile mancante.
    """
    pass


class Espressione:
    """
    - CLASSE ASTRATTA ESPRESSIONE: Rappresenta un'espressione generica.
    
    # Perché astratta?
    - Non ha senso creare una "Espressione" generica senza tipo.
    - Forza le sottoclassi a implementare valuta() e __str__().
    """
    
    def __init__(self):
        """
        - INIT: Espressione non può essere istanziata direttamente.
        """
        raise NotImplementedError("Non istanziare Espressione direttamente")
    
    @classmethod
    def da_programma(cls, testo, tabella):
        """
        - DA_PROGRAMMA: Costruisce un'espressione dalla notazione polacca inversa.
        
        # Parametri:
        - testo: stringa come "2 3 + x *"
        - tabella: dizionario {operatore: classe} es. {"+": Addizione}
        
        # Algoritmo:
        1. Divido la stringa in pezzi (separatore: spazio)
        2. Per ogni pezzo:
           - Se è un intero o float → creo Costante
           - Se è nella tabella → operatore (arity dalla classe)
           - Altrimenti → Variabile
        3. Alla fine, la pila deve contenere UN solo elemento → l'espressione
        
        # Restituisce:
        - Un oggetto di tipo Espressione (sottoclasse concreta)
        """
        # 1. INIZIALIZZO LA PILA
        pila = Pila()
        
        # 2. DIVIDO LA STRINGA IN PEZZI: Il separatore è lo spazio
        pezzi = testo.split()
        
        # 3. PROCESSO OGNI PEZZO
        for pezzo in pezzi:

            # 3.1 PEZZO NUMERICO: Provo a convertirlo in int o float
            try:
                # Provo come int prima (perché l'esempio usa interi)
                if '.' in pezzo:
                    valore = float(pezzo)
                else:
                    valore = int(pezzo)
                pila.push(Costante(valore))
                continue
            except ValueError:
                pass  # Non è un numero, continuo con gli altri controlli
            
            # 3.2 PEZZO OPERATORE: Verifico se è nella tabella
            if pezzo in tabella:
                # Ottengo la classe dell'operatore (es. Addizione)
                classe_op = tabella[pezzo]
                
                # Controllo se la classe ha l'attributo 'arity'
                if not hasattr(classe_op, 'arita'):
                    raise ValueError(f"La classe {classe_op.__name__} non ha l'attributo 'arita'")
                
                arita = classe_op.arita
                
                # Estraggo gli argomenti dalla pila
                argomenti = []
                for _ in range(arita):
                    if pila.dati == []:
                        raise ValueError(f"Pila vuota durante la costruzione di {pezzo}")
                    argomenti.append(pila.pop())
                
                # Inverto l'ordine degli argomenti:
                # - Perché la pila è LIFO: l'ultimo pezzo letto è il primo argomento
                # - Ma per operazioni non commutative è importante l'ordine originale.
                argomenti.reverse()
                
                # Creo l'operazione e la pusho nella pila
                operazione = classe_op(argomenti)
                pila.push(operazione)
            else:
                # 3.3 PEZZO VARIABILE: Tutto ciò che non è numero né operatore
                pila.push(Variabile(pezzo))
        
        # 4. CONTROLLO FINALE: La pila deve contenere ESATTAMENTE UN elemento
        if len(pila.dati) != 1:
            raise ValueError(f"Espressione malformata: la pila contiene {len(pila.dati)} elementi")
        
        return pila.pop()
    
    def valuta(self, ambiente):
        """
        - VALUTA: Valuta l'espressione – deve essere implementato dalle sottoclassi.
        """
        raise NotImplementedError()


class Variabile(Espressione):
    """
    --- CLASSE VARIABILE ---
    Rappresenta una variabile (es. x, y, z).
    
    Durante la valutazione, il valore viene letto dall'ambiente (dizionario ambiente).
    """
    
    def __init__(self, nome):
        """
        - INIT: nome: stringa con il nome della variabile (es. "x")
        """
        self.nome = nome
    
    def valuta(self, ambiente):
        """
        - VALUTA: Restituisce il valore della variabile dall'ambiente.
        
        # Perché VariabileMancanteException?
        - La consegna lo richiede esplicitamente.
        - Permette di distinguere tra "variabile non definita" e altri errori.
        """
        # 1. CONTROLLO ESISTENZA: se la variabile non è nell'ambiente
        if self.nome not in ambiente:
            raise VariabileMancanteException(f"Variabile '{self.nome}' non definita nell'ambiente")
        
        # 2. RESTITUISCO IL VALORE
        return ambiente[self.nome]
    
    def __str__(self):
        """
        - STR: Restituisce il nome della variabile.
        
        # Perché non il valore?
        - La rappresentazione testuale deve mostrare l'espressione, non il risultato.
        """
        return self.nome


class Costante(Espressione):
    """
    - CLASSE COSTANTE: Rappresenta un valore numerico costante.
    
    Le costanti non dipendono dall'ambiente di valutazione.
    """
    
    def __init__(self, valore):
        """
        - INIT: valore: numero (int o float)
        """
        self.valore = valore
    
    def valuta(self, ambiente):
        """
        - VALUTA: Le costanti ignorano l'ambiente e restituiscono se stesse.
        """
        return self.valore
    
    def __str__(self):
        """
        - STR: Restituisce il valore come stringa.
        """
        return str(self.valore)



# ============================================================================
# CLASSI PER LE OPERAZIONI
# ============================================================================

class Operazione(Espressione):
    """
    - CLASSE ASTRATTA OPERAZIONE: Rappresenta un'operazione generica (binaria o unaria).
    
    # Attributo di classe:
    - arita: numero di argomenti (da definire nelle sottoclassi)
    """
    
    arita = None  # Deve essere sovrascritto dalle sottoclassi
    
    def __init__(self, argomenti):
        """
        - INIT: argomenti: lista di oggetti Espressione (gli operandi)
        """
        self.argomenti = argomenti
    
    def valuta(self, ambiente):
        """
        - VALUTA: Valuta l'operazione.
        
        # Algoritmo:
        1. Prima valuta ricorsivamente tutti gli argomenti
        2. Poi applica l'operazione (operazione) sui valori ottenuti
        
        # Perché separato tra valuta e operazione?
        - valuta gestisce la ricorsione (albero delle espressioni)
        - operazione implementa il calcolo specifico (es. somma, prodotto)
        - Permette di non duplicare la logica di ricorsione in ogni operazione
        """
        # 1. VALUTO TUTTI GLI ARGOMENTI: Ricorsione sull'albero
        valori = [arg.valuta(ambiente) for arg in self.argomenti]
        
        # 2. APPLICO L'OPERAZIONE: Chiamo operazione con i valori valutati
        return self.operazione(*valori)
    
    def operazione(self, *args):
        """
        - OPERAZIONE: Implementa l'operazione specifica (es. somma, prodotto).
        
        # Deve essere sovrascritto dalle sottoclassi concrete.
        
        # Perché *args?
        - Permette di gestire sia operazioni binarie (2 argomenti)
        - Che operazioni unarie (1 argomento)
        """
        raise NotImplementedError()
    
    def __str__(self):
        """
        - STR: Rappresentazione testuale dell'operazione.
        
        # Formato: (operatore arg1 arg2 ...)
        
        # Perché uso self.simbolo?
        - La consegna vuole il simbolo dell'operatore (es. "+", "*", "1/")
        - Non il nome della classe (es. "Addizione")
        """
        # 1. PRENDO LA RAPPRESENTAZIONE DEGLI ARGOMENTI
        argomenti_str = " ".join(str(arg) for arg in self.argomenti)
        
        # 2. COSTRUISCO LA STRINGA FINALE
        return f"({self.simbolo} {argomenti_str})"


class OperazioneBinaria(Operazione):
    """
    - CLASSE OPERAZIONE BINARIA: Operazione binaria (arita = 2).
    
    # Perché una classe separata?
    - Definisce arita=2 una volta per tutte.
    - Non serve riscriverlo in ogni operazione binaria.
    """
    arita = 2


class OperazioneUnaria(Operazione):
    """
    - CLASSE OPERAZIONE UNARIA: Operazione unaria (arita = 1).
    
    # Perché una classe separata?
    - Definisce arita=1 una volta per tutte.
    - Non serve riscriverlo in ogni operazione unaria.
    """
    arita = 1


# ============================================================================
# OPERAZIONI BINARIE CONCRETE
# ============================================================================

class Addizione(OperazioneBinaria):
    """
    - ADDIZIONE: Operazione di addizione: a + b
    """
    simbolo = "+"
    
    def operazione(self, a, b):
        return a + b


class Sottrazione(OperazioneBinaria):
    """
    - SOTTRAZIONE: Operazione di sottrazione: a - b
    """
    simbolo = "-"
    
    def operazione(self, a, b):
        return a - b


class Moltiplicazione(OperazioneBinaria):
    """
    - MOLTIPLICAZIONE: Operazione di moltiplicazione: a * b
    """
    simbolo = "*"
    
    def operazione(self, a, b):
        return a * b


class Divisione(OperazioneBinaria):
    """
    - DIVISIONE: Operazione di divisione: a / b
    """
    simbolo = "/"
    
    def operazione(self, a, b):
        # Nota: in Python 3, / restituisce sempre float
        return a / b


class Potenza(OperazioneBinaria):
    """
    - POTENZA: Operazione di potenza: a ** b
    """
    simbolo = "**"
    
    def operazione(self, a, b):
        return a ** b


class Modulo(OperazioneBinaria):
    """
    - MODULO: Operazione di modulo (resto): a % b
    """
    simbolo = "%"
    
    def operazione(self, a, b):
        return a % b


# ============================================================================
# OPERAZIONI UNARIE CONCRETE
# ============================================================================

class Reciproco(OperazioneUnaria):
    """
    - RECIPROCO: Operazione di reciproco: 1 / a
    """
    simbolo = "1/"
    
    def operazione(self, a):
        return 1 / a


class ValoreAssoluto(OperazioneUnaria):
    """
    - VALORE ASSOLUTO: Operazione di valore assoluto: |a|
    """
    simbolo = "abs"
    
    def operazione(self, a):
        return abs(a)


# ============================================================================
# TABELLA DI DISPATCH (associazione simbolo → classe)
# ============================================================================

tabella_dispatch = {
    "+": Addizione,
    "*": Moltiplicazione,
    "**": Potenza,
    "-": Sottrazione,
    "/": Divisione,
    "%": Modulo,
    "1/": Reciproco,
    "abs": ValoreAssoluto,
}


# ==========================
# SEZIONE DI TEST 
# ==========================

if __name__ == "__main__":
    print("=" * 60)
    print("TEST ESPRESSIONI - Parte B")
    print("=" * 60)
    
    # -------------------------------------------------------------------------
    # 1. ESPRESSIONE SEMPLICE (solo costanti)
    # -------------------------------------------------------------------------
    print("\n[1] ESPRESSIONE SEMPLICE (2 3 +)")
    
    esp1 = Espressione.da_programma("2 3 +", tabella_dispatch)
    print(f"    RPN: 2 3 +")
    print(f"    Albero: {esp1}")
    print(f"    Risultato: {esp1.valuta({})}")
    print(f"    → Atteso: 5")
    
    # -------------------------------------------------------------------------
    # 2. ESPRESSIONE CON VARIABILE
    # -------------------------------------------------------------------------
    print("\n[2] ESPRESSIONE CON VARIABILE (2 3 + x *)")
    
    esp2 = Espressione.da_programma("2 3 + x *", tabella_dispatch)
    print(f"    RPN: 2 3 + x *")
    print(f"    Albero: {esp2}")
    print(f"    Con x=4: {esp2.valuta({'x': 4})}")
    print(f"    → Atteso: (2+3)*4 = 20")
    
    # -------------------------------------------------------------------------
    # 3. ESPRESSIONE CON OPERATORE UNARIO (reciproco)
    # -------------------------------------------------------------------------
    print("\n[3] ESPRESSIONE CON OPERATORE UNARIO (2 1/)")
    
    esp3 = Espressione.da_programma("2 1/", tabella_dispatch)
    print(f"    RPN: 2 1/")
    print(f"    Albero: {esp3}")
    print(f"    Risultato: {esp3.valuta({})}")
    print(f"    → Atteso: 0.5")
    
    # -------------------------------------------------------------------------
    # 4. ESPRESSIONE CON OPERATORE UNARIO (valore assoluto)
    # -------------------------------------------------------------------------
    print("\n[4] ESPRESSIONE CON OPERATORE UNARIO (-3 abs)")
    
    esp4 = Espressione.da_programma("-3 abs", tabella_dispatch)
    print(f"    RPN: -3 abs")
    print(f"    Albero: {esp4}")
    print(f"    Risultato: {esp4.valuta({})}")
    print(f"    → Atteso: 3")
    
    # -------------------------------------------------------------------------
    # 5. ESPRESSIONE COMPLESSA (esempio della consegna)
    # -------------------------------------------------------------------------
    print("\n[5] ESPRESSIONE COMPLESSA (esempio del professore)")
    
    esempio = "2 3 + x * 6 5 - / abs 2 ** y 1/ + 1/"
    print(f"    RPN: {esempio}")
    
    esp_complessa = Espressione.da_programma(esempio, tabella_dispatch)
    print(f"    Albero: {esp_complessa}")
    
    risultato = esp_complessa.valuta({"x": 3, "y": 7})
    print(f"    Con x=3, y=7 → {risultato}")
    print(f"    → Atteso: 0.84022932953024")
    
    # -------------------------------------------------------------------------
    # 6. TEST VARIABILE MANCANTE (gestione eccezione)
    # -------------------------------------------------------------------------
    print("\n[6] TEST VARIABILE MANCANTE")
    
    try:
        risultato_mancante = esp_complessa.valuta({"x": 3})  # y manca
        print(f"    ERRORE: doveva sollevare eccezione, invece ha restituito {risultato_mancante}")
    except VariabileMancanteException as e:
        print(f"    Eccezione sollevata (corretta): {e}")
    
    # -------------------------------------------------------------------------
    # 7. TEST PILA VUOTA (espressione malformata)
    # -------------------------------------------------------------------------
    print("\n[7] TEST ESPRESSIONE MALFORMATA (troppi operatori)")
    
    try:
        esp_malformata = Espressione.da_programma("2 3 + +", tabella_dispatch)
        print(f"    ERRORE: doveva sollevare eccezione, invece ha funzionato")
    except ValueError as e:
        print(f"    Eccezione sollevata (corretta): {e}")
    
    # -------------------------------------------------------------------------
    # 8. TEST ESPRESSIONE VUOTA
    # -------------------------------------------------------------------------
    print("\n[8] TEST ESPRESSIONE VUOTA")
    
    try:
        esp_vuota = Espressione.da_programma("", tabella_dispatch)
        print(f"    ERRORE: doveva sollevare eccezione, invece ha funzionato")
    except ValueError as e:
        print(f"    Eccezione sollevata (corretta): {e}")
    
    print("\n" + "=" * 60)
    print("TEST COMPLETATI CON SUCCESSO")
    print("=" * 60)