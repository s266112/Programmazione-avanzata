// Alessio Matessi  S266112

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "tensorforth.h"

// Lunghezza massima di un singolo token del sorgente
#define MAX_TOKEN 4096

/* --- IS_FLOAT_TOKEN ---
    -> Ritorna 1 se il token e' un numero valido (es. "-8.5"), 0 altrimenti.
*/
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

/* --- ESEGUI_OPERATORE ---
    -> Esegue l'operatore 'op' sullo stack.
    -> Ritorna 1 se l'operatore esiste, 0 se non e' riconosciuto.
*/
static int esegui_operatore(Stack* stack, char op)
{
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
        case '(': op_leggi_pgm(stack); break;
        case ')': op_scrivi_pgm(stack); break;
        case '{': op_leggi_binario(stack); break;
        case '}': op_scrivi_binario(stack); break;

        default:
            return 0;
    }
    return 1;
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
    if (argc > 2)
    {
        fprintf(stderr, "Uso: %s [nome file sorgente]\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (argc == 2)
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
    char token[MAX_TOKEN];
    char formato_token[32];
    snprintf(formato_token, sizeof(formato_token), "%%%ds", MAX_TOKEN - 1);

    float* temp_array = NULL;   // Numeri letti tra [ ] (array dinamico)
    size_t temp_count = 0;
    size_t temp_capacita = 0;
    int in_array = 0;           // Flag: Attivo se sto leggendo i numeri dentro le parentesi [ ]
    int esito = EXIT_SUCCESS;

    // 5. Ciclo principale di lettura: Analizza ogni parola separata da spazi o a capo.
    //    In caso di errore esco dal ciclo, cosi' la memoria viene liberata al punto 6.
    while (esito == EXIT_SUCCESS && fscanf(input_file, formato_token, token) == 1)
    {
        // Rilevamento inizio tensore '['. Da questo punto, i numeri successivi vengono raccolti in temp_array.
        if (strcmp(token, "[") == 0)
        {
            // I tensori annidati non esistono in TensorForth: '[' dentro un array e' un errore
            if (in_array)
            {
                fprintf(stderr, "Errore: '[' annidata, i tensori non possono contenere altri tensori.\n");
                esito = EXIT_FAILURE;
                break;
            }
            in_array = 1;
            temp_count = 0;
        }
        // Rilevamento fine tensore ']'. Quando chiudo l'array, lo converto in un tensore 1D e lo pusho nello stack.
        else if (strcmp(token, "]") == 0)
        {
            // Una ']' senza la '[' corrispondente e' un errore di sintassi
            if (!in_array)
            {
                fprintf(stderr, "Errore: ']' senza '[' corrispondente.\n");
                esito = EXIT_FAILURE;
                break;
            }
            if (temp_count == 0)
            {
                fprintf(stderr, "Errore: tensore vuoto '[ ]' non ammesso.\n");
                esito = EXIT_FAILURE;
                break;
            }

            // Creo il tensore 1D con i numeri accumulati
            int32_t forma[1] = { (int32_t)temp_count };
            Tensore* t = crea_tensore(1, forma);
            memcpy(t->dati, temp_array, temp_count * sizeof(float));
            push(stack, t);
            in_array = 0;
        }
        // Elaborazione dei numeri (dentro un blocco [ ]): sono ammessi solo numeri
        else if (in_array)
        {
            if (!is_float_token(token))
            {
                fprintf(stderr, "Errore: token non valido in array '%s'.\n", token);
                esito = EXIT_FAILURE;
                break;
            }

            // Se l'array e' pieno ne raddoppio la dimensione
            if (temp_count == temp_capacita)
            {
                size_t nuova_capacita = (temp_capacita == 0) ? 64 : temp_capacita * 2;
                float* nuovo = (float*)realloc(temp_array, nuova_capacita * sizeof(float));
                if (nuovo == NULL)
                {
                    perror("Errore: impossibile allocare memoria per il tensore");
                    esito = EXIT_FAILURE;
                    break;
                }
                temp_array = nuovo;
                temp_capacita = nuova_capacita;
            }
            temp_array[temp_count++] = (float)strtod(token, NULL);
        }
        // Gestione delle stringhe (nomi dei file tra doppi apici): vanno sullo stack
        else if (token[0] == '"')
        {
            size_t len = strlen(token);
            if (len > 2 && token[len - 1] == '"')
            {
                token[len - 1] = '\0';              // tolgo l'apice finale
                push_stringa(stack, token + 1);     // e salto quello iniziale
            }
            else
            {
                fprintf(stderr, "Errore: stringa filename non valida '%s'.\n", token);
                esito = EXIT_FAILURE;
                break;
            }
        }
        // Identificazione ed esecuzione degli operatori.
        else if (strlen(token) == 1)
        {
            if (!esegui_operatore(stack, token[0]))
            {
                fprintf(stderr, "Errore: Operatore '%c' non riconosciuto.\n", token[0]);
                esito = EXIT_FAILURE;
                break;
            }
        }
        else
        {
            fprintf(stderr, "Errore: token non riconosciuto '%s'.\n", token);
            esito = EXIT_FAILURE;
            break;
        }
    }

    if (esito == EXIT_SUCCESS && in_array)
    {
        fprintf(stderr, "Errore: array non chiuso. Manca ] .\n");
        esito = EXIT_FAILURE;
    }

    // 6. Chiusura delle risorse e pulizia finale della memoria
    if (input_file != stdin)
    {
        fclose(input_file);
    }
    free(temp_array);
    libera_stack(stack);

    return esito;
}
