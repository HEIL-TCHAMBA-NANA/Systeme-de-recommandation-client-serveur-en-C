#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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


float **Generate_matrix_knn(Transaction *transactions, int num_transactions, int m, int n) {
    // Trouver les user_id et item_id uniques et créer des mappings
    int *user_ids = (int *)malloc(m * sizeof(int));
    int *item_ids = (int *)malloc(n * sizeof(int));
    int user_count = 0, item_count = 0;

    // Remplir les tableaux d'IDs uniques
    for (int i = 0; i < num_transactions; i++) {
        int found_user = 0, found_item = 0;
        for (int j = 0; j < user_count; j++) {
            if (user_ids[j] == transactions[i].user_id) {
                found_user = 1;
                break;
            }
        }
        if (!found_user) {
            user_ids[user_count++] = transactions[i].user_id;
        }
        for (int j = 0; j < item_count; j++) {
            if (item_ids[j] == transactions[i].item_id) {
                found_item = 1;
                break;
            }
        }
        if (!found_item) {
            item_ids[item_count++] = transactions[i].item_id;
        }
    }

    // Allocation de la matrice
    float **matrix = (float **)malloc(m * sizeof(float *));
    for (int i = 0; i < m; i++) {
        matrix[i] = (float *)calloc(n, sizeof(float)); // Initialisation à 0
    }

    // Remplissage de la matrice avec les transactions
    for (int i = 0; i < num_transactions; i++) {
        int user_index = -1, item_index = -1;
        for (int j = 0; j < user_count; j++) {
            if (user_ids[j] == transactions[i].user_id) {
                user_index = j;
                break;
            }
        }
        for (int j = 0; j < item_count; j++) {
            if (item_ids[j] == transactions[i].item_id) {
                item_index = j;
                break;
            }
        }
        if (user_index != -1 && item_index != -1) {
            matrix[user_index][item_index] = transactions[i].rating;
        }
    }

    free(user_ids);
    free(item_ids);

    return matrix;
}
