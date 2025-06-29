#include "function_principale.h"

// Fonction pour charger les transactions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "function_principale.h"

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