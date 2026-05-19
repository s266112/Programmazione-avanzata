from collections import defaultdict


class Polinomio:

    # =========================
    # COSTRUTTORE
    # =========================

    def __init__(self, coeffs):
        """
        - INIT: coeffs: dizionario {grado: coefficiente}
        
        Perché defaultdict(int)?
        - Quando accedo a un grado non presente, restituisce 0 automaticamente.
        - Perfetto per i polinomi (x^5 che non c'è → coefficiente 0).
        """
        
        # 1. INIZIALIZZO UN DEFAULTDICT
        self._coeffs = defaultdict(int)     # int è una funzione che restituisce 0, quando chiamata senza argomenti

        # 2. COPIO I COEFFICENTI FORNITI
        for grado, coeff in coeffs.items():
            self._coeffs[grado] = coeff


    
    # ===========================================
    # GETITEM E SETITEM (ACCESSO AI COEFFICENTI) 
    # ===========================================

    def __getitem__(self, k):
        """
        - GETITEM: Restituisce il coefficiente del grado k.
        
        # Uso: p[3] → coefficiente di x^3
        # Perché funziona?: defaultdict restituisce 0 se k non esiste -> Comportamento corretto
        """
        
        return self._coeffs[k]
    

    def __setitem__(self, k, v):
        """
        - SETITEM: Imposta il coefficiente del grado k a v.
        
        # Uso: p[3] = 2.4
        # Ottimizzazione: se v == 0; cancello la chiave per mantenere il dizionario pulito. 
        """

        if v == 0:
            # Se il grado esiste lo rimuovo (coeff zero non serve in memoria)
            if k in self._coeffs:
                del self._coeffs[k]
        else:
            self._coeffs[k] = v

    

    # ==================================
    # VALUTAZIONE DEL POLINOMIO
    # ==================================
    
    def __call__(self, x):
        """
        - CALL: Calcola il valore del polinomio nel punto x.
        
        # Formula: Σ (coefficiente * x^grado)

        # Perché x ** grado?    
        - Leggibile e adeguato per polinomi di grado non enorme.
        - Python gestisce la potenza in modo nativo.
        """

        # 1. PARTENZA: Risultato iniziale = 0
        risultato = 0.0
        
        # 2. SOMMO TERMINE PER TERMINE
        for grado, coeff in self._coeffs.items():
            risultato += coeff * (x ** grado)
        
        return risultato
    

    
    # ========================================================================
    # OPERAZIONI DI POLINOMI
    # ========================================================================

# -- Somma --    
    def __add__(self, altro):       
        """
        - ADD: Restituisce un NUOVO polinomio (self + altro).
        
        # Cosa può essere altro? 
        - Un altro Polinomio
        - Un numero (int/float) → viene trattato come costante (grado 0)
        
        # Perché restituisco un nuovo oggetto?  
        - Lo chiede la consegna
        - Non modifico self (immutabilità).
        """
        # 1. CONVERSIONE: Se altro è un numero, diventa Polinomio costante
        if isinstance(altro, (int, float)):
            altro = Polinomio({0: altro})
        
        # 2. CONTROLLO TIPO: Se non è Polinomio, errore
        if not isinstance(altro, Polinomio):
            raise TypeError(f"Impossibile sommare Polinomio con {type(altro)}")
        
        # 3. UNIONE DEI GRADI: Prendo tutti i gradi presenti in almeno un polinomio
        tutti_gradi = set(self._coeffs.keys()) | set(altro._coeffs.keys())
        
        # 4. COSTRUISCO IL DIZIONARIO DEI COEFFICIENTI SOMMA
        nuovi_coeffs = {}
        for grado in tutti_gradi:
            somma = self[grado] + altro[grado]  # self[grado] usa __getitem__
            if somma != 0:                      # Evito di salvare coefficienti nulli
                nuovi_coeffs[grado] = somma
        
        # 5. RESTITUISCO UN NUOVO POLINOMIO
        return Polinomio(nuovi_coeffs)



# -- Sottrazione --
    def __sub__(self, altro):
        """
        -SUB: Restituisce un NUOVO polinomio (self - altro).
        
        # Strategia: self - altro = self + (-altro)
        - Evito di duplicare la logica di addizione.
        - Basta creare un polinomio "negato" di altro e sommarlo.
        
        # Cosa può essere altro?
        - Un altro Polinomio
        - Un numero (int/float) → trattato come costante
        """
        # 1. CONVERSIONE: Se altro è un numero, diventa Polinomio costante
        if isinstance(altro, (int, float)):
            altro = Polinomio({0: altro})
        
        # 2. CONTROLLO TIPO
        if not isinstance(altro, Polinomio):
            raise TypeError(f"Impossibile sottrarre {type(altro)} da Polinomio")
        
        # 3. CREO IL POLINOMIO NEGATO: Moltiplico tutti i coefficienti per -1
        coeffs_negati = {}
        for grado, coeff in altro._coeffs.items():
            if -coeff != 0:                         # Evito di salvare zeri
                coeffs_negati[grado] = -coeff
        
        # 4. SOMMA: Self + (-altro) usando __add__ già implementato
        return self + Polinomio(coeffs_negati)
    


# -- Moltiplicazione --
    def __mul__(self, altro):
        """
        - MUL: Restituisce un NUOVO polinomio (self * altro).
        
        # Algoritmo: Prodotto di due polinomi = somma di tutti i prodotti 
                   tra ogni termine di self e ogni termine di altro.
        
        # Regola: (coeff_a * x^grado_a) * (coeff_b * x^grado_b) = 
                  (coeff_a * coeff_b) * x^(grado_a + grado_b)
        
       # Cosa può essere altro?
        - Un altro Polinomio
        - Un numero (int/float) → prodotto scalare
        """
        # 1. CONVERSIONE: Se altro è un numero, lo trasformo in Polinomio costante
        if isinstance(altro, (int, float)):
            altro = Polinomio({0: altro})
        
        # 2. CONTROLLO TIPO
        if not isinstance(altro, Polinomio):
            raise TypeError(f"Impossibile moltiplicare Polinomio con {type(altro)}")
        
        # 3. DIZIONARIO PROVVISORIO PER I RISULTATI (uso dict normale, non defaultdict)
        #    Perché? Perché devo sommare più contributi allo stesso grado.
        risultati = {}
        
        # 4. DOPPIO CICLO: Per ogni termine di self e ogni termine di altro
        for grado_a, coeff_a in self._coeffs.items():
            for grado_b, coeff_b in altro._coeffs.items():

                # 4.1 Calcolo grado e coefficiente del prodotto
                grado_prodotto = grado_a + grado_b
                coeff_prodotto = coeff_a * coeff_b
                
                # 4.2 Sommo al grado corrispondente (possono esserci più contributi)
                if grado_prodotto in risultati:
                    risultati[grado_prodotto] += coeff_prodotto
                else:
                    risultati[grado_prodotto] = coeff_prodotto
        
        # 5. PULIZIA: Rimuovo i coefficienti nulli (per economia di memoria)
        coeffs_finali = {}
        for grado, coeff in risultati.items():
            if coeff != 0:
                coeffs_finali[grado] = coeff
        
        # 6. RESTITUISCO UN NUOVO POLINOMIO
        return Polinomio(coeffs_finali)

       

# -- Elevamento a potenza --
    def __pow__(self, n):
        """
        - POW: Restituisce un NUOVO polinomio (self ** n), dove n è un intero non negativo.
        
        # Algoritmo: Elevamento a potenza tramite moltiplicazioni ripetute.
        Esempio: p ** 3 = p * p * p
        
        # Perché non uso ottimizzazioni tipo esponenziazione binaria?
        - n (l'esponente) è tipicamente piccolo in questa esercitazione.
        - La consegna suggerisce di sfruttare __mul__ già implementato.
        - Codice più semplice e leggibile.
        
        # Cosa succede se n == 0?
        - Qualsiasi polinomio elevato a 0 dà il polinomio costante 1.
        
        # Cosa succede se n < 0?
        - Non gestito (potenza negativa non è un polinomio). Sollevo un errore.
        """
        # 1. CONTROLLO: n deve essere intero
        if not isinstance(n, int):
            raise TypeError(f"L'esponente deve essere intero, non {type(n)}")
        
        # 2. CASO n == 0: Restituisco il polinomio costante 1
        if n == 0:
            return Polinomio({0: 1})
        
        # 3. CASO n < 0: Non supportato (darebbe polinomio con termini negativi)
        if n < 0:
            raise ValueError("Potenza negativa non supportata per i polinomi")
        
        # 4. CASO GENERALE: Moltiplicazioni ripetute
        #    Parto dal polinomio originale e moltiplico n-1 volte per self
        risultato = self  # p^1
        for _ in range(1, n):             # eseguo n-1 moltiplicazioni
            risultato = risultato * self  # uso __mul__
        
        return risultato
    
   
# -- Derivata del polinomio -- 
    def derivata(self):
        """
        - DERIVATA: Restituisce un NUOVO polinomio che è la derivata di self.
        
        # Regola di derivazione: d/dx [c * x^k] = c * k * x^(k-1)
        
        # Cosa succede al termine costante (grado 0)?
        - La derivata di una costante è 0 → il termine scompare.
        
        # Perché restituisco un nuovo oggetto?
        - La consegna richiede: "ritornare un oggetto di tipo Polinomio".
        - Non modifico self (immutabilità).
        """
        # 1. DIZIONARIO PER I COEFFICIENTI DELLA DERIVATA
        coeffs_derivata = {}
        
        # 2. APPLICO LA REGOLA DI DERIVAZIONE A OGNI TERMINE
        for grado, coeff in self._coeffs.items():

            # 2.1 Se grado == 0: Derivata = 0 → non aggiungo nulla (salta)
            if grado == 0:
                continue   # Il termine costante sparisce
            
            # 2.2 Nuovo coefficiente = coeff * grado
            #     Nuovo grado = grado - 1
            nuovo_grado = grado - 1
            nuovo_coeff = coeff * grado
            
            # 2.3 Salvo solo se il coefficiente è diverso da zero
            if nuovo_coeff != 0:
                coeffs_derivata[nuovo_grado] = nuovo_coeff
        
        # 3. RESTITUISCO UN NUOVO POLINOMIO
        return Polinomio(coeffs_derivata)
    

    # ========================================================================
    # RAPPRESENTAZIONE IN STRINGA
    # ========================================================================
    def __str__(self):
        """
        - STR: Restituisce una stringa che rappresenta il polinomio in formato leggibile.
        
        # Formato richiesto: "2x^4 + 5x^3 + 6x^1 + -2x^0"

        # Devo fare attenzione ai dettagli:
        - I termini sono ordinati dal grado maggiore al minore.
        - Il coefficiente 1 viene mostrato come "1x^k" (non solo "x^k").
        - Il coefficiente -1 viene mostrato come "-1x^k".
        - L'esponente 1 viene mostrato: "x^1" (non "x").
        - L'esponente 0 viene mostrato: "x^0" (parte costante).
        """

        # 1. CASO POLINOMIO VUOTO (tutti coefficienti zero)
        if not self._coeffs:
            return "0"
        
        # 2. OTTENGO I GRADI IN ORDINE DECRESCENTE
        #    Perché? Perché matematicamente scrivo prima i termini di grado più alto.
        gradi_ordinati = sorted(self._coeffs.keys(), reverse=True)
        
        # 3. COSTRUISCO LA LISTA DEI TERMINI
        termini = []
        for grado in gradi_ordinati:
            coeff = self._coeffs[grado]
            
            # 3.1 Costruisco il termine come stringa
            if grado == 0:
                # Termine costante: solo il coefficiente
                termine = f"{coeff}"
            else:
                # Termine con x: coefficiente * x^grado
                termine = f"{coeff}x^{grado}"
            
            termini.append(termine)
        
        # 4. UNISCO I TERMINI CON " + " (anche se i coefficienti sono negativi)
        return " + ".join(termini)


# ==============================================================
# FUNZIONE NEWTON-RAPHSON (fuori dalla classe Polinomio)
# ==============================================================

def newton_raphson(p, x, n_iter=20):
    """
    - NEWTON-RAPHSON: Trova uno zero del polinomio p partendo da una stima iniziale x.
    
    # Formula iterativa: x_{n+1} = x_n - p(x_n) / p'(x_n)
    
    # Parametri:
    - p: un oggetto Polinomio
    - x: stima iniziale (float o int)
    - n_iter: numero di iterazioni (default 20)
    
    Restituisce l'approssimazione finale dello zero (valore di x dopo n_iter iterazioni)
    
    # Perché non controllo la convergenza?
    - La consegna richiede un numero fisso di iterazioni (n_iter).
    - Semplice e prevedibile per il test.
    
    # Cosa uso di Polinomio?
    - p(x) → __call__
    - p.derivata() → metodo derivata
    - p.derivata()(x) → __call__ sulla derivata
    """
    # 1. CALCOLO UNA VOLTA LA DERIVATA (non serve ricalcolarla ogni iterazione)
    derivata = p.derivata()
    
    # 2. ITERO PER n_iter VOLTE
    for _ in range(n_iter):

        # 2.1 Valuto il polinomio e la sua derivata nel punto corrente
        valore_p = p(x)
        valore_derivata = derivata(x)
        
        # 2.2 Se la derivata è zero, non posso dividere (evito divisione per zero)
        if valore_derivata == 0:
            # In questo caso, la formula non è applicabile. Restituisco x corrente.
            return x
        
        # 2.3 Aggiorno x secondo Newton-Raphson
        x = x - valore_p / valore_derivata
    
    return x


# ============================================================================
# SEZIONE DI TEST 
# ============================================================================

if __name__ == "__main__":
    print("=" * 60)
    print("TEST POLINOMI - Parte A")
    print("=" * 60)
    
    # -------------------------------------------------------------------------
    # 1. CREAZIONE DEI POLINOMI
    # -------------------------------------------------------------------------
    p1 = Polinomio({4: 2, 3: 5, 1: 6, 0: -2})
    p2 = Polinomio({3: 1, 2: 0.5, 1: -1, 0: 3})
    
    print("\n[1] POLINOMI DI PARTENZA")
    print(f"    p1(x) = {p1}")
    print(f"    p2(x) = {p2}")
    
    # -------------------------------------------------------------------------
    # 2. SOMMA
    # -------------------------------------------------------------------------
    print("\n[2] SOMMA (p1 + p2)")
    print(f"    (p1 + p2)(x) = {p1 + p2}")
    
    # -------------------------------------------------------------------------
    # 3. PRODOTTO
    # -------------------------------------------------------------------------
    print("\n[3] PRODOTTO (p1 * p2)")
    print(f"    (p1 * p2)(x) = {p1 * p2}")
    
    # -------------------------------------------------------------------------
    # 4. VALUTAZIONE IN UN PUNTO
    # -------------------------------------------------------------------------
    x_val = 3
    print(f"\n[4] VALUTAZIONE p1({x_val})")
    print(f"    p1({x_val}) = {p1(x_val)}")
    print(f"    Verifica manuale: 2*81 + 5*27 + 6*3 - 2 = 162 + 135 + 18 - 2 = 313")
    
    # -------------------------------------------------------------------------
    # 5. VALUTAZIONE DELLA SOMMA IN UN PUNTO
    # -------------------------------------------------------------------------
    x_val = 20
    p1_p2_somma = p1 + p2
    print(f"\n[5] VALUTAZIONE (p1 + p2)({x_val})")
    print(f"    (p1 + p2)({x_val}) = {p1_p2_somma(x_val)}")
    
    # -------------------------------------------------------------------------
    # 6. DERIVATA
    # -------------------------------------------------------------------------
    print("\n[6] DERIVATA di p1")
    print(f"    p1'(x) = {p1.derivata()}")
    print(f"    Verifica: 2x^4 → 8x^3, 5x^3 → 15x^2, 6x^1 → 6, -2 → 0")
    
    # -------------------------------------------------------------------------
    # 7. NEWTON-RAPHSON (ricerca dello zero)
    # -------------------------------------------------------------------------
    print("\n[7] METODO DI NEWTON-RAPHSON")
    print("    Cerco uno zero di p1(x) partendo da x0 = 5")
    
    zero = newton_raphson(p1, 5, n_iter=20)
    valore_zero = p1(zero)
    
    print(f"    Approssimazione zero: x ≈ {zero}")
    print(f"    p1({zero}) = {valore_zero}")
    print(f"    Quanto vicino a zero? |p1(x)| = {abs(valore_zero):.2e}")
    
    # -------------------------------------------------------------------------
    # 8. TEST AGGIUNTIVI (verifica consistenza)
    # -------------------------------------------------------------------------
    print("\n[8] TEST AGGIUNTIVI")
    
    # Test sottrazione
    print(f"    p1 - p2 = {p1 - p2}")
    
    # Test elevamento a potenza
    print(f"    p1^2 = {p1 ** 2}")
    print(f"    p1^0 = {p1 ** 0}  (polinomio costante 1)")
    
    # Test accesso ai coefficienti
    print(f"    Coefficiente di x^3 in p1: p1[3] = {p1[3]}")
    print(f"    Coefficiente di x^2 in p1: p1[2] = {p1[2]} (non presente → 0)")
    
    # Modifica di un coefficiente
    p1[2] = 10
    print(f"    Dopo p1[2] = 10 → p1(x) = {p1}")
    p1[2] = 0  # ripristino
    print(f"    Dopo p1[2] = 0  → p1(x) = {p1}")
    
    print("\n" + "=" * 60)
    print("TEST COMPLETATI CON SUCCESSO")
    print("=" * 60)