# Esercitazione 10: Polinomi ed Espressioni in Python

## 🚀 OBIETTIVO DEL PROGETTO
Implementare una classe `Polinomio` per la gestione di polinomi in una variabile e un sistema per costruire e valutare espressioni in notazione polacca inversa (RPN) tramite una gerarchia di classi.

---

## 📊 ANALISI LOGICA (STRUTTURA DATI)

### Parte A – Polinomio
La classe `Polinomio` utilizza un `defaultdict(int)` per memorizzare i coefficienti. I gradi non presenti restituiscono automaticamente 0.

**Operazioni supportate**:
- Somma, sottrazione, prodotto, potenza
- Valutazione in un punto (`__call__`)
- Derivata
- Accesso e modifica dei coefficienti (`__getitem__`, `__setitem__`)
- Metodo di Newton-Raphson per la ricerca degli zeri

### Parte B – Espressioni RPN
La notazione polacca inversa scrive prima gli operandi, poi l'operatore (es. `2 3 +` = `2+3`).

**Gerarchia delle classi**:
- `Espressione` (astratta) → `Costante`, `Variabile`, `Operazione`
- `Operazione` → `OperazioneBinaria` (+, -, *, /, **, %) e `OperazioneUnaria` (1/, abs)

**Costruzione**: tramite pila (stack) a partire da una stringa RPN e un dizionario `tabella` che associa simboli a classi.

**Valutazione**: ricorsiva, con gestione di `VariabileMancanteException` se una variabile non è definita nell'ambiente.

---

## 🛠️ NOTE TECNICHE

- **Polinomio immutabile**: le operazioni restituiscono nuovi oggetti
- **Coefficienti nulli**: vengono rimossi per risparmiare memoria
- **Newton-Raphson**: implementato come funzione esterna (non metodo) con numero fisso di iterazioni
- **Gerarchia con `arita`**: evita duplicazione del codice per operazioni binarie/unari

---

## 📂 ESECUZIONE

**Parte A – Polinomi**:
```bash
python3 "Parte A - polinomi.py"
```

**Parte B – Espressioni RPN**:
```bash
python3 "Parte A - espressioni.py"
```