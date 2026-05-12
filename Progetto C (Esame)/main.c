// Alessio Matessi  S266112

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tensorforth.h"

// Dimensione massima di sicurezza per gli array letti tra parentesi quadre
#define MAX_ARRAY_SIZE 10000 

/* --- PARSER TENSORFORTH --- 
   -> Legge un file sorgente o l'input da terminale;
   -> Elabora i token uno per volta applicando la logica stack-based;
   -> Gestisce la creazione di tensori, stringhe e l'esecuzione di operazioni.
*/
int main(int argc, char *argv[])
{
    // 1. Inizializzazione del generatore di numeri casuali
    srand(time(NULL));

    // 2. Gestione dell'input (File sorgente o Standard Input)
    FILE* input_file = stdin; 
    
    // Se viene passato un argomento da riga di comando, tento di aprirlo come file
    if (argc > 1) 
    {
        input_file = fopen(argv[1], "r");
        if (!input_file) 
        {
            fprintf(stderr, "Errore: impossibile aprire il file sorgente %s\n", argv[1]);
            return EXIT_FAILURE;
        }
    }

    // 3. Inizializzazione dello stack di lavoro
    Stack* stack = crea_stack();

    // 4. Variabili di supporto per il parsing dei token
    char token[256];
    float temp_array[MAX_ARRAY_SIZE];
    int temp_count = 0;
    int in_array = 0;           // Flag: attivo se stiamo leggendo i numeri dentro le parentesi [ ]
    char temp_filename[256] = "";

    // 5. Ciclo principale di lettura: analizza ogni parola separata da spazi o a capo
    while (fscanf(input_file, "%255s", token) == 1)
    {
        // Rilevamento inizio tensore '['
        if (strcmp(token, "[") == 0) 
        {
            in_array = 1;
            temp_count = 0;
        }
        // Rilevamento fine tensore ']'
        else if (strcmp(token, "]") == 0) 
        {
            if (in_array) 
            {
                // Creo il tensore 1D con i numeri accumulati
                int32_t forma[1] = { temp_count };
                Tensore* t = crea_tensore(1, forma);
                for (int i = 0; i < temp_count; i++) 
                {
                    t->dati[i] = temp_array[i];
                }
                push(stack, t);
                in_array = 0;
            }
        }
        // Gestione delle stringhe (Nomi dei file tra doppi apici)
        else if (token[0] == '"') 
        {
            // Estraggo il nome del file eliminando i doppi apici
            size_t len = strlen(token);
            if (len > 2)
            {
                strncpy(temp_filename, token + 1, len - 2);
                temp_filename[len - 2] = '\0';
            }
        }
        // Elaborazione dei numeri (dentro un blocco [ ])
        else if ((token[0] >= '0' && token[0] <= '9') || (token[0] == '-' && token[1] >= '0')) 
        {
            if (in_array && temp_count < MAX_ARRAY_SIZE) 
            {
                temp_array[temp_count++] = (float)atof(token);
            }
        }
        // Identificazione ed esecuzione degli operatori
        else if (strlen(token) == 1) 
        {
            char op = token[0];
            switch (op) 
            {
                // Operazioni Aritmetiche
                case '+': op_somma(stack); break;
                case '-': op_sottrazione(stack); break;
                case '*': op_prodotto(stack); break;
                
                // Comparazioni e Logica
                case '<': op_minore(stack); break;
                case '>': op_maggiore(stack); break;
                case '=': op_uguale(stack); break;
                case '&': op_and(stack); break;
                case '|': op_or(stack); break;
                case '!': op_not(stack); break;
                
                // Operazioni Speciali e Matematiche
                case '$': op_selezione(stack); break;
                case '@': op_prodotto_matrici(stack); break;
                case '.': op_prodotto_interno(stack); break;
                case 'c': op_convoluzione_2d(stack); break;
                case '~': op_ravel(stack); break;
                case '#': op_shape(stack); break;
                case 'r': op_reshape(stack); break;
                case '?': op_random(stack); break;
                case 'R': op_relu(stack); break;
                case 'm': op_minimo(stack); break;
                case 'M': op_massimo(stack); break;
                case 'S': op_somma_riduzione(stack); break;
                case 'f': op_fill(stack); break;
                
                // Gestione Stack e Utilità
                case 'p': op_stampa(stack); break;
                case 'd': op_duplica(stack); break;
                case 's': op_scambia(stack); break;
                case 'D': op_drop(stack); break;
                case 'o': op_over(stack); break;
                
                // Input / Output
                case '(': op_leggi_pgm(stack, temp_filename); break;
                case ')': op_scrivi_pgm(stack, temp_filename); break;
                case '{': op_leggi_binario(stack, temp_filename); break;
                case '}': op_scrivi_binario(stack, temp_filename); break;
                
                default:
                    fprintf(stderr, "Errore: Operatore '%c' non riconosciuto.\n", op);
                    break;
            }
        }
    }

    // 6. Chiusura delle risorse e pulizia finale della memoria
    if (input_file != stdin) 
    {
        fclose(input_file);
    }
    libera_stack(stack);

    return EXIT_SUCCESS;
}