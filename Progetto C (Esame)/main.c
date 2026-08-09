// Alessio Matessi  S266112

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "tensorforth.h"

// Dimensione massima di sicurezza per gli array letti tra parentesi quadre
#define MAX_ARRAY_SIZE 10000 

static int is_float_token(const char* token)
{
    if (token == NULL || token[0] == '\0')
    {
        return 0;
    }

    char* endptr;
    errno = 0;
    strtod(token, &endptr);
    return errno == 0 && *endptr == '\0' && endptr != token;
}

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
    int in_array = 0;           // Flag: Attivo se sto leggendo i numeri dentro le parentesi [ ]
    char temp_filename[256] = "";

    // 5. Ciclo principale di lettura: Analizza ogni parola separata da spazi o a capo.
    //    Il parser è uno stato semplice: legge token, costruisce array, memorizza filename
    //    e applica operatori stack-based uno per uno.
    while (fscanf(input_file, "%255s", token) == 1)
    {
        // Rilevamento inizio tensore '['. Da questo punto, i numeri successivi vengono raccolti in temp_array.
        if (strcmp(token, "[") == 0) 
        {
            in_array = 1;
            temp_count = 0;
        }
        // Rilevamento fine tensore ']'. Quando chiudo l'array, lo converto in un tensore 1D e lo pusho nello stack.
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
        // Gestione delle stringhe (nomi dei file tra doppi apici).
        // Il token viene memorizzato in temp_filename e utilizzato dal prossimo operatore di I/O.
        else if (token[0] == '"') 
        {
            size_t len = strlen(token);
            if (len > 2 && token[len - 1] == '"')
            {
                strncpy(temp_filename, token + 1, len - 2);
                temp_filename[len - 2] = '\0';
            }
            else
            {
                fprintf(stderr, "Errore: stringa filename non valida '%s'.\n", token);
                return EXIT_FAILURE;
            }
        }
        // Elaborazione dei numeri (dentro un blocco [ ])
        else if (in_array && is_float_token(token))
        {
            if (temp_count < MAX_ARRAY_SIZE)
            {
                temp_array[temp_count++] = (float)strtod(token, NULL);
            }
            else
            {
                fprintf(stderr, "Errore: dimensione dell'array supera il limite %d.\n", MAX_ARRAY_SIZE);
                return EXIT_FAILURE;
            }
        }
        // Identificazione ed esecuzione degli operatori.
        // Ogni singolo token di lunghezza 1 è trattato come un operatore Forth-style.
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
                case '~':
                case '_': op_ravel(stack); break;
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
                case '(': 
                    if (temp_filename[0] == '\0')
                    {
                        fprintf(stderr, "Errore: filename mancante prima di '('.\n");
                        return EXIT_FAILURE;
                    }
                    op_leggi_pgm(stack, temp_filename);
                    temp_filename[0] = '\0';
                    break;
                case ')': 
                    if (temp_filename[0] == '\0')
                    {
                        fprintf(stderr, "Errore: filename mancante prima di ')'.\n");
                        return EXIT_FAILURE;
                    }
                    op_scrivi_pgm(stack, temp_filename);
                    temp_filename[0] = '\0';
                    break;
                case '{': 
                    if (temp_filename[0] == '\0')
                    {
                        fprintf(stderr, "Errore: filename mancante prima di '{'.\n");
                        return EXIT_FAILURE;
                    }
                    op_leggi_binario(stack, temp_filename);
                    temp_filename[0] = '\0';
                    break;
                case '}': 
                    if (temp_filename[0] == '\0')
                    {
                        fprintf(stderr, "Errore: filename mancante prima di '}'.\n");
                        return EXIT_FAILURE;
                    }
                    op_scrivi_binario(stack, temp_filename);
                    temp_filename[0] = '\0';
                    break;
                
                default:
                    fprintf(stderr, "Errore: Operatore '%c' non riconosciuto.\n", op);
                    return EXIT_FAILURE;
            }
        }
        else
        {
            if (in_array)
            {
                fprintf(stderr, "Errore: token non valido in array '%s'.\n", token);
                return EXIT_FAILURE;
            }
            else
            {
                fprintf(stderr, "Errore: token non riconosciuto '%s'.\n", token);
                return EXIT_FAILURE;
            }
        }
    }

    if (in_array)
    {
        fprintf(stderr, "Errore: array non chiuso. Mancano ] .\n");
        if (input_file != stdin)
        {
            fclose(input_file);
        }
        libera_stack(stack);
        return EXIT_FAILURE;
    }

    // 6. Chiusura delle risorse e pulizia finale della memoria
    if (input_file != stdin) 
    {
        fclose(input_file);
    }
    libera_stack(stack);

    return EXIT_SUCCESS;
}