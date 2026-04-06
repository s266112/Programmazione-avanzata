# Lezione 1 - Introduzione

## 📌 Prerequisiti

Si assumono conoscenze di base di programmazione in **C** (*puntatori*, memoria dinamica, strutture, funzioni) e **Python**.
Sono richieste anche basi di *architettura dei calcolatori*, sistemi operativi, multithreading, algoritmi e strutture dati.

---

## 📌 Esame

L’esame è composto da **progetto**, **scritto** e **orale**.
Il progetto vale il **50%**, mentre scritto e orale il restante **50%**.

Per accedere all’orale è necessario superare sia progetto che scritto.

Il progetto è individuale ed è diviso in due parti:

* una in **C**
* una in **Python**

---

## 📌 Struttura del calcolatore

Un programma viene eseguito dalla CPU attraverso il ciclo: **richiesta → decodifica → esecuzione**.

Le CPU moderne sono più complesse e permettono di eseguire più istruzioni contemporaneamente, grazie a diverse unità interne.

---

## 📌 Accesso alla memoria

La **memoria (RAM)** è molto più lenta della CPU.
Per ridurre questo problema esiste una gerarchia di **cache** (*L1, L2, L3*), più veloci ma più piccole.

In generale:

* **L1** → molto veloce, molto piccola
* **L2**
* **L3** → più grande ma più lenta

I dati vengono trasferiti a **blocchi**, quindi accedere a indirizzi vicini è più efficiente.

---

## 📌 Accessi con stride

Se si accede agli elementi in modo consecutivo (`stride = 1`), si sfrutta la **cache** e l’esecuzione è veloce.
Se invece si accede con salti (`stride` elevato), si perde la *località di memoria* e le prestazioni peggiorano.

---

## 💻 Codice

```c
int readmem(char * m, int num, int stride)
{
  int sum = 0;
  for (int i = 0; i < num; i++) 
  {
    sum += m[stride * i];
  }
  return sum;
}
```

👉 `stride` indica quanto si salta nella memoria:

* `stride` piccolo → accessi vicini → **cache sfruttata** → veloce
* `stride` grande → accessi lontani → cache inefficiente → lento

---

## 📌 Branch prediction

Le CPU cercano di prevedere il risultato delle istruzioni condizionali (*branch prediction*).

* previsione corretta → esecuzione veloce
* previsione sbagliata → rallentamento

Se i dati sono ordinati, il comportamento è prevedibile.
Se i dati sono casuali, diventa difficile prevedere e le prestazioni peggiorano.

---

## 💻 Codice

```c
int sum_small_values(int * v, int len)
{
  int sum = 0;
  for (int i = 0; i < len; i++) 
  {
    if (v[i] < 50) 
    {
      sum += v[i];
    }
  }
  return sum;
}
```

👉 il tempo dipende dai dati:

* dati ordinati → `if` prevedibile → più veloce
* dati casuali → comportamento imprevedibile → più lento

---

## 📌 Moltiplicazione di matrici

La complessità è **O(n³)**, ma le prestazioni reali dipendono dall’implementazione.

In particolare:

* **Python** → più lento
* **C** → più veloce
* **NumPy** → molto più veloce (*librerie ottimizzate*)

---

## 💻 Codice

```c
C[i][j] += A[i][k] * B[k][j];
```

👉 triplo ciclo annidato → costo **O(n³)**

```python
c = np.dot(a, b)
```

👉 `NumPy` è più veloce perché:

* usa codice in **C**
* sfrutta librerie ottimizzate (*BLAS*)
* utilizza meglio **cache** e CPU

---

## 📌 Idea fondamentale

Le performance non dipendono solo dall’algoritmo, ma anche da:

* accesso alla memoria
* uso della **cache**
* *branch prediction*
* ottimizzazione dell’implementazione

# Lezione 2 - Organizzare progetti in C

## 📌 Compilazione semplice

Fino ad ora abbiamo lavorato con un singolo file in cui tutto il codice è definito nello stesso posto.

```c
#include <stdio.h>

struct stat 
{
  int min;
  int max;
  float avg;
};

struct stat compute_stats(int *v, int len)
{
  struct stat s;
  s.min = v[0];
  s.max = v[0];
  s.avg = 0;
  for (int i = 0; i < len; i++) 
  {
    if (v[i] < s.min) 
    {
      s.min = v[i];
    }
    if (v[i] > s.max) 
    {
      s.max = v[i];
    }
    s.avg += v[i];
  }
  s.avg /= len;
  return s;
}

int main(int argc, char * argv[])
{
  int v[8] = {2, 3, 5, 7, -8, 9, 4, 1};
  struct stat s = compute_stats(v, 8);
  printf("min: %d\tmax: %d\tavg: %f\n", s.min, s.max, s.avg);
  return 0;
}
```

👉 questo è il caso base: tutto in un unico file.

Compilazione:

```bash
gcc -o main main.c
```

---

## 📌 Problema: codice riutilizzabile

Se vogliamo usare la funzione `compute_stats` in più programmi, non possiamo copiare il codice ogni volta.

Serve quindi separare il codice in più file.

---

## 📌 Separazione in più file

Possiamo dividere il codice:

* `main.c` → contiene il `main`
* `stat.c` → contiene la funzione

Esempio `main.c`:

```c
#include <stdio.h>

struct stat 
{
  int min;
  int max;
  float avg;
};

extern struct stat compute_stats(int * v, int len);

int main(int argc, char * argv[])
{
  int v[8] = {2, 3, 5, 7, -8, 9, 4, 1};
  struct stat s = compute_stats(v, 8);
  printf("min: %d\tmax: %d\tavg:%f\n", s.min, s.max, s.avg);
  return 0;
}
```

👉 qui usiamo `extern` per dire che la funzione esiste in un altro file.

---

## 📌 Implementazione separata

```c
struct stat 
{
  int min;
  int max;
  float avg;
};

struct stat compute_stats(int *v, int len)
{
  struct stat s;
  s.min = v[0];
  s.max = v[0];
  s.avg = 0;

  for (int i = 0; i < len; i++) 
  {
    if (v[i] < s.min) 
    {
      s.min = v[i];
    }
    if (v[i] > s.max) 
    {
      s.max = v[i];
    }
    s.avg += v[i];
  }

  s.avg /= len;
  return s;
}
```

👉 questa è la definizione reale della funzione (`stat.c`).

---

## 📌 Compilazione separata

```bash
gcc -c main.c
gcc -c stat.c
gcc -o main main.o stat.o
```

👉 `-c` crea i file oggetto (`.o`) senza fare linking.

---

## 📌 Problema delle struct duplicate

Se definiamo la `struct` in più file, dobbiamo stare attenti.

Se l’ordine dei campi cambia, il programma si rompe:

```c
struct stat 
{
  float avg;
  int min;
  int max;
};
```

👉 anche se sembra uguale, la memoria è diversa → risultati sbagliati.

---

## 📌 Idea

Serve un modo per:

* definire struct e funzioni **una sola volta**
* usarle in tutti i file

👉 soluzione: **file header (.h)**

## 📌 File header (.h)

Per evitare problemi con `extern` e duplicazioni, si usano i file header (`.h`).

Un file header contiene:

* definizione delle **struct**
* dichiarazione delle **funzioni**

Esempio:

```c id="j2w3ka"
#ifndef _STAT_H
#define _STAT_H

struct stat 
{
  int min;
  int max;
  float avg;
};

struct stat compute_stats(int * v, int len);

#endif
```

👉 questo file:

* definisce la `struct`
* dichiara la funzione
* evita doppie inclusioni (con `#ifndef`)

---

## 📌 Uso degli header

Ora nei file `.c` non scriviamo più tutto, ma includiamo l’header.

### stat.c

```c id="xg6mzn"
#include "stat.h"

struct stat compute_stats(int *v, int len)
{
  struct stat s;
  s.min = v[0];
  s.max = v[0];
  s.avg = 0;

  for (int i = 0; i < len; i++) 
  {
    if (v[i] < s.min) 
    {
      s.min = v[i];
    }
    if (v[i] > s.max) 
    {
      s.max = v[i];
    }
    s.avg += v[i];
  }

  s.avg /= len;
  return s;
}
```

👉 qui implementiamo la funzione dichiarata nell’header.

---

### main.c

```c id="o7m5zi"
#include <stdio.h>
#include "stat.h"

int main(int argc, char * argv[])
{
  int v[8] = {2, 3, 5, 7, -8, 9, 4, 1};
  struct stat s = compute_stats(v, 8);
  printf("min: %d\tmax: %d\tavg: %f\n", s.min, s.max, s.avg);
  return 0;
}
```

👉 qui usiamo la funzione senza `extern`, perché è già dichiarata nell’header.

---

## 📌 Vantaggi degli header

* una sola definizione della `struct`
* niente errori di duplicazione
* codice più ordinato
* separazione tra **interfaccia (.h)** e **implementazione (.c)**

---

## 📌 Include guard

Il blocco:

```c id="sk5p3d"
#ifndef _STAT_H
#define _STAT_H
...
#endif
```

serve per evitare che lo stesso header venga incluso più volte.

👉 senza questo, potremmo avere errori di compilazione.

---

## 📌 Estensione: più file

Possiamo aggiungere altri moduli, ad esempio per stampare vettori.

### Header

```c id="y4r7pw"
#ifndef _PRINT_VECTOR_H
#define _PRINT_VECTOR_H

void print_vector_int(int * v, int len);
void print_vector_float(float * v, int len);

#endif
```

---

### Implementazione

```c id="g6tr4n"
#include <stdio.h>
#include "print_vector.h"

void print_vector_int(int * v, int len)
{
  for (int i = 0; i < len; i++) {
    printf("%d", v[i]);
    if (i == len - 1) printf("\n");
    else printf(" ");
  }
}

void print_vector_float(float * v, int len)
{
  for (int i = 0; i < len; i++) 
  {
    printf("%f", v[i]);
    if (i == len - 1) printf("\n");
    else printf(" ");
  }
}
```

---

### Uso nel main

```c id="o5kp4b"
#include <stdio.h>
#include "stat.h"
#include "print_vector.h"

int main(int argc, char * argv[])
{
  int v[8] = {2, 3, 5, 7, -8, 9, 4, 1};

  print_vector_int(v, 8);

  struct stat s = compute_stats(v, 8);

  printf("min: %d\tmax: %d\tavg: %f\n", s.min, s.max, s.avg);
  return 0;
}
```

👉 ora il progetto è modulare:

* un file per ogni funzionalità
* header per collegarli

## 📌 Librerie statiche

Quando vogliamo riutilizzare codice in più progetti, possiamo creare una **libreria statica** (`.a`).

L’idea è:

* compiliamo i file `.c` → otteniamo `.o`
* li uniamo in una libreria
* poi li colleghiamo al programma

---

## 💻 Creazione libreria statica

```bash
gcc -c stat.c
ar r libstat.a stat.o
ranlib libstat.a
```

👉 cosa succede:

* `gcc -c` → crea `stat.o`
* `ar` → crea la libreria `libstat.a`
* `ranlib` → crea l’indice della libreria

---

## 💻 Uso della libreria

```bash
gcc -L. -o main main.c -lstat
```

👉 importante:

* `-L.` → cerca la libreria nella directory corrente
* `-lstat` → usa `libstat.a` (senza `lib` e `.a`)

---

## 📌 Librerie dinamiche

Le librerie dinamiche (`.so`, `.dylib`, `.dll`) vengono caricate **a runtime**.

Vantaggi:

* non serve ricompilare tutto se cambia la libreria
* più programmi possono usare la stessa libreria
* meno spazio occupato

---

## 💻 Creazione libreria dinamica

```bash
gcc -fPIC -c stat.c
gcc -o libstat.dylib -shared stat.o
gcc -o main main.c -L. -lstat
```

👉 `-fPIC` serve per creare codice indipendente dalla posizione in memoria

---

## 💻 Esecuzione

```bash
export DYLD_LIBRARY_PATH=./lib/:$DYLD_LIBRARY_PATH
./main
```

👉 serve per dire al sistema dove trovare la libreria

---

## 📌 Differenza statiche vs dinamiche

* **statiche (.a)** → incluse nel programma al momento del linking
* **dinamiche (.so/.dylib)** → caricate quando il programma viene eseguito

---

## 📌 Idea chiave

Le librerie servono per:

* riutilizzare codice
* organizzare progetti grandi
* separare sviluppo e utilizzo del codice

## 📌 Perché serve make

Quando i progetti crescono e abbiamo molti file `.c` e `.h`, compilare tutto a mano diventa scomodo.

Se modifichiamo un file:

* dobbiamo ricompilare solo quello
* poi rifare il linking

👉 non ha senso ricompilare tutto ogni volta

Serve quindi un sistema automatico.

---

## 📌 Idea di make

Vogliamo specificare tre cose:

* **target** → cosa vogliamo ottenere (es: `main`)
* **prerequisiti** → da cosa dipende (es: `.o`, `.c`, `.h`)
* **comando** → come costruirlo

---

## 📌 Esempio logico

* target: `main`
* prerequisiti: `main.o`, `stat.o`
* comando:

```bash
gcc -o main main.o stat.o
```

---

## 📌 Makefile base

Un Makefile contiene regole nel formato:

```makefile
target: prerequisiti
    comando
```

⚠️ ATTENZIONE: il comando deve iniziare con **TAB, non spazi**

---

## 💻 Esempio

```makefile
all: main

main: main.o stat.o
	gcc -o main main.o stat.o

stat.o: stat.c stat.h
	gcc -c stat.c

main.o: main.c stat.h
	gcc -c main.c
```

👉 questo Makefile:

* costruisce `main`
* ricompila solo ciò che serve

---

## 📌 Come funziona make

Quando eseguiamo:

```bash
make
```

make:

* controlla i prerequisiti
* se sono cambiati → ricompila
* se non sono cambiati → non fa nulla

👉 esempio:

* cambi `stat.c` → ricompila solo `stat.o` + linking
* non cambi nulla → niente da fare

---

## 📌 Invocazione

* `make` → esegue il target principale (`all`)
* `make main` → costruisce solo `main`
* `make -f nomefile` → usa un file diverso

---

## 📌 Target clean

Un target comune è `clean`:

```makefile
clean:
	rm *.o
```

👉 serve per eliminare file intermedi

---

## 📌 .PHONY

```makefile
.PHONY: clean
```

👉 indica che `clean` non è un file reale
👉 evita problemi se esiste un file chiamato `clean`

---

## 📌 Variabili

Possiamo usare variabili per evitare ripetizioni:

```makefile
CC = gcc
CFLAGS = -O3

main:
	$(CC) $(CFLAGS) -o main main.o stat.o
```

👉 `$(CC)` e `$(CFLAGS)` vengono sostituiti

---

## 📌 Variabili automatiche

Alcune variabili sono automatiche:

* `$@` → target
* `$^` → tutti i prerequisiti
* `$<` → primo prerequisito

---

## 📌 Regole template

Possiamo scrivere regole generiche:

```makefile
%.o: %.c
	gcc -c $<
```

👉 significa:

* per ogni file `.o`, serve il corrispondente `.c`

---

## 📌 Wildcard

Possiamo prendere tutti i file:

```makefile
SRC = $(wildcard *.c)
```

👉 prende tutti i file `.c` nella directory

---

## 📌 Attenzione

* usare **TAB**, non spazi
* ogni riga è eseguita separatamente
* make è molto semplice (anni ‘70), quindi va usato con attenzione

---

## 📌 Idea fondamentale

Make serve per:

* automatizzare la compilazione
* ricompilare solo ciò che serve
* gestire progetti grandi

## 📌 Ottimizzazione del compilatore

Il compilatore può applicare diverse ottimizzazioni per rendere il codice più veloce.

I livelli principali sono:

* `-O0` → nessuna ottimizzazione
* `-O1`
* `-O2`
* `-O3` → massimo livello (di solito)

---

## 💻 Esempio

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char * argv[])
{
  srand(time(NULL));
  int n = 1<<20;
  int *v = (int *) malloc(n * sizeof(int));

  for (int i = 0; i < n; i++) {
    v[i] = rand() % 10;
  }

  int dot = 0;

  clock_t start = clock();
  for (int i = 0; i < n; i++) {
    dot += v[i] * v[i];
  }
  clock_t end = clock();

  float ms = (end - start) / (CLOCKS_PER_SEC/1000.0);

  printf("il prodotto interno di v è %d\n", dot);
  printf("il tempo richiesto è stato %f ms\n", ms);

  return 0;
}
```

👉 calcola un prodotto interno e misura il tempo

---

## 💻 Compilazione con diversi livelli

```bash
gcc -O0 -o main0 main.c
gcc -O1 -o main1 main.c
gcc -O2 -o main2 main.c
gcc -O3 -o main3 main.c
gcc -O3 -march=native -o mainN main.c
```

👉 `-march=native` usa tutte le ottimizzazioni disponibili sulla macchina

---

## 📌 Idea

A parità di codice:

* il livello di ottimizzazione può cambiare molto le performance

---

## 📌 Debug

Per il debug si usa:

```bash
-g
```

👉 mantiene informazioni utili (nomi variabili, ecc.)
👉 utile con debugger (es. `gdb`)

---

## 📌 Warning del compilatore

Il compilatore può segnalare problemi nel codice.

Opzioni principali:

* `-Wall` → warning base
* `-Wextra` → più warning
* `-pedantic` → rispetto standard

---

## 💻 Esempio

```c
#include <stdio.h>
#include <stdlib.h>

int f(int x, int y)
{
  return x*x;
}

int main(int argc, char * argv[])
{
  short a[3] = {1,2,3};
  int b = 1<<64;
  printf("%d\n", b);

  for (int i = 0; i < 3; i++) {
    printf("%d %d\t", a[i], f(a[i], 1));
  }

  int * p = malloc(sizeof(int));
  printf("%p\n", p);

  return 0;
}
```

👉 questo codice genera warning perché:

* `1<<64` → overflow
* possibile uso scorretto dei tipi
* puntatori non gestiti

---

## 📌 Standard del C

Possiamo scegliere la versione del C:

* `-std=c89`
* `-std=c99`
* `-std=c11`
* `-std=c17`

👉 serve per garantire compatibilità

---

## 📌 Idea fondamentale

Le opzioni del compilatore servono per:

* migliorare le performance (`-O`)
* fare debug (`-g`)
* trovare errori (`-Wall`, `-Wextra`)
* controllare lo standard (`-std`)




