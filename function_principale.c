#include "function_principale.h"
#include "structures/struct_transaction.h"

// Fonction pour charger les transactions
Transaction *load_transactions(const char *filename, int *num_transactions, int *m, int *n) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        *num_transactions = 0;
        *m = 0;
        *n = 0;
        return NULL;
    }

    int max_users = 1000;
    int max_items = 1000;
    int *user_map = (int *)calloc(max_users, sizeof(int));
    if (!user_map) {
        perror("Erreur d'allocation pour user_map");
        fclose(file);
        *num_transactions = *m = *n = 0;
        return NULL;
    }
    int *item_map = (int *)calloc(max_items, sizeof(int));
    if (!item_map) {
        perror("Erreur d'allocation pour item_map");
        free(user_map);
        fclose(file);
        *num_transactions = *m = *n = 0;
        return NULL;
    }
    int local_m = 0, local_n = 0;
    int count = 0;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        count++;
        int u_id, i_id, c_id;
        float r;
        long ts;
        if (sscanf(line, "%d%d%d%f%ld", &u_id, &i_id, &c_id, &r, &ts) != 5) {
            fprintf(stderr, "Ligne %d mal formée : %s", count, line);
            continue;
        }
        // Ajout : Vérification des indices négatifs
        if (u_id < 0 || i_id < 0) {
            fprintf(stderr, "Indice négatif à la ligne %d\n", count);
            continue;
        }

        while (u_id >= max_users) {
            max_users *= 2;
            int *new_user_map = (int *)realloc(user_map, max_users * sizeof(int));
            if (!new_user_map) {
                perror("Erreur de réallocation pour user_map");
                free(user_map);
                free(item_map);
                fclose(file);
                *num_transactions = *m = *n = 0;
                return NULL;
            }
            user_map = new_user_map;
            memset(user_map + max_users / 2, 0, max_users / 2 * sizeof(int));
        }
        if (!user_map[u_id]) {
            user_map[u_id] = 1;
            local_m++;
        }

        while (i_id >= max_items) {
            max_items *= 2;
            int *new_item_map = (int *)realloc(item_map, max_items * sizeof(int));
            if (!new_item_map) {
                perror("Erreur de réallocation pour item_map");
                free(user_map);
                free(item_map);
                fclose(file);
                *num_transactions = *m = *n = 0;
                return NULL;
            }
            item_map = new_item_map;
            memset(item_map + max_items / 2, 0, max_items / 2 * sizeof(int));
        }
        if (!item_map[i_id]) {
            item_map[i_id] = 1;
            local_n++;
        }
    }

    *num_transactions = count;
    *m = local_m;
    *n = local_n;

    Transaction *transactions = (Transaction *)malloc(count * sizeof(Transaction));
    if (!transactions) {
        perror("Erreur d'allocation mémoire");
        free(user_map);
        free(item_map);
        fclose(file);
        *num_transactions = 0;
        *m = 0;
        *n = 0;
        return NULL;
    }

    rewind(file);
    int index = 0;
    int line_number = 0;
    while (fgets(line, sizeof(line), file)) {
        line_number++;
        int u_id, i_id, c_id;
        float r;
        long ts;
        if (sscanf(line, "%d%d%d%f%ld", &u_id, &i_id, &c_id, &r, &ts) != 5) {
            fprintf(stderr, "Ignorer ligne %d mal formée : %s", line_number, line);
            continue;
        }
        // Ajout : Vérification des indices négatifs
        if (u_id < 0 || i_id < 0) {
            fprintf(stderr, "Indice négatif à la ligne %d\n", line_number);
            continue;
        }
        transactions[index].user_id = u_id;
        transactions[index].item_id = i_id;
        transactions[index].category_id = c_id;
        transactions[index].rating = r;
        transactions[index].timestamp = ts;
        index++;
    }

    *num_transactions = index;

    if (index < count) {
        Transaction *new_transactions = (Transaction *)realloc(transactions, index * sizeof(Transaction));
        if (!new_transactions) {
            perror("Erreur de réallocation pour transactions");
            free(transactions);
            free(user_map);
            free(item_map);
            fclose(file);
            *num_transactions = 0;
            return NULL;
        }
        transactions = new_transactions;
    }

    free(user_map);
    free(item_map);
    fclose(file);
    return transactions;
}


long date_to_timestamp(const char *date_str) {
    struct tm date = {0};
    int day, month, year;

    // Parser la date au format JJ/MM/AAAA
    if (sscanf(date_str, "%d/%d/%d", &day, &month, &year) != 3) {
        fprintf(stderr, "Format de date invalide. Utilisez JJ/MM/AAAA\n");
        return -1;
    }

    // Valider les plages
    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1970) {
        fprintf(stderr, "Date invalide : jour [1-31], mois [1-12], année >= 1970\n");
        return -1;
    }

    // Remplir la structure tm
    date.tm_mday = day;
    date.tm_mon = month - 1;    // Mois : 0-11
    date.tm_year = year - 1900; // Année depuis 1900
    date.tm_hour = 0;
    date.tm_min = 0;
    date.tm_sec = 0;
    date.tm_isdst = -1;         // Gestion automatique de l'heure d'été

    // Convertir en timestamp
    time_t timestamp = mktime(&date);
    if (timestamp == -1) {
        fprintf(stderr, "Erreur de conversion ");
        return -1;
    }

    return (long)timestamp;
}


void periodic_transaction(const char *filename, const char *debut_str, const char *fin_str){
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    FILE *output_file = fopen("transactions_periodiques.txt", "w");
    if (!output_file) {
        perror("Erreur lors de l'ouverture du fichier de sortie");
        fclose(file);
        return;
    }

    long debut_ts = date_to_timestamp(debut_str);
    long fin_ts = date_to_timestamp(fin_str);
    if (debut_ts == -1 || fin_ts == -1) {
        fclose(file);
        fclose(output_file);
        return; // Erreur de conversion de date
    }

    while (1) {
        char line[256];
        if (!fgets(line, sizeof(line), file)) {
            break; // Fin du fichier
        }

        int u_id, i_id, c_id;
        float r;
        long ts;
        if (sscanf(line, "%d%d%d%f%ld", &u_id, &i_id, &c_id, &r, &ts) != 5) {
            fprintf(stderr, "Ligne mal formée : %s", line);
            continue;
        }
        // Vérification de l'intervalle de temps
        if (ts >= debut_ts && ts <= fin_ts) {
            fprintf(output_file, "%s", line);
        }
    }
    
}

void delete_transaction(const char *filename, int min_U, int min_I) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file) {
        perror("Erreur lors de l'ouverture du fichier temporaire");
        fclose(file);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        int u_id, i_id, c_id;
        float r;
        long ts;
        if (sscanf(line, "%d%d%d%f%ld", &u_id, &i_id, &c_id, &r, &ts) != 5) {
            fprintf(stderr, "Ligne mal formée : %s", line);
            continue;
        }

        // Vérification des conditions de suppression
        if (u_id < min_U && i_id < min_I) {
            continue; // Ne pas écrire cette ligne dans le fichier temporaire
        }
        fprintf(temp_file, "%s", line);
    }

    fclose(file);
    fclose(temp_file);

    // Remplacer l'ancien fichier par le nouveau
    remove(filename);
    rename("temp.txt", filename);
}

void clean_test_file(const char *training, const char *test) {
    FILE *train_file = fopen(training, "r");
    if (!train_file) {
        perror("Erreur lors de l'ouverture du fichier d'entraînement");
        return;
    }

    FILE *test_file = fopen(test, "r");
    if (!test_file) {
        perror("Erreur lors de l'ouverture du fichier de test");
        fclose(train_file);
        return;
    }

    FILE *temp_file = fopen("temp_test.txt", "w");
    if (!temp_file) {
        perror("Erreur lors de l'ouverture du fichier temporaire");
        fclose(train_file);
        fclose(test_file);
        return;
    }

    // Charger toutes les transactions du fichier train en mémoire (sous forme de chaînes)
    // On suppose que le nombre de transactions est raisonnable pour la RAM
    #define MAX_LINES 1000000
    #define LINE_SIZE 256
    char **train_lines = malloc(MAX_LINES * sizeof(char *));
    int train_count = 0;
    char line[LINE_SIZE];
    while (fgets(line, sizeof(line), train_file)) {
        train_lines[train_count] = strdup(line);
        train_count++;
        if (train_count >= MAX_LINES) break;
    }

    // Pour chaque ligne du fichier test, vérifier si elle existe dans train
    char test_line[LINE_SIZE];
    while (fgets(test_line, sizeof(test_line), test_file)) {
        int found = 0;
        for (int i = 0; i < train_count; i++) {
            if (strcmp(test_line, train_lines[i]) == 0) {
                found = 1;
                break;
            }
        }
        if (found) {
            fputs(test_line, temp_file);
        }
    }

    // Libération mémoire
    for (int i = 0; i < train_count; i++) {
        free(train_lines[i]);
    }
    free(train_lines);

    fclose(train_file);
    fclose(test_file);
    fclose(temp_file);

    // Remplacer le fichier test par le fichier temporaire nettoyé
    remove(test);
    rename("temp_test.txt", test);
}

void get_top_n_recommendations(float **full_matrix, int user_id, int m, int n, int N, int *top_items) {
    typedef struct { int item_id; float rating; } ItemRating;
    ItemRating *ratings = malloc(n * sizeof(ItemRating));
    if (!ratings) return;
    for (int j = 0; j < n; j++) {
        ratings[j].item_id = j;
        ratings[j].rating = full_matrix[user_id][j];
    }
    // Tri par note décroissante (tri à bulles simple)
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (ratings[j].rating < ratings[j + 1].rating) {
                ItemRating temp = ratings[j];
                ratings[j] = ratings[j + 1];
                ratings[j + 1] = temp;
            }
        }
    }
    for (int i = 0; i < N && i < n; i++) {
        top_items[i] = ratings[i].item_id;
    }
    free(ratings);
}
