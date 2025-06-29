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

    // Étape 1 : Compter le nombre de lignes et estimer m, n
    int max_users = 1000;  // Taille initiale pour les utilisateurs
    int max_items = 1000;  // Taille initiale pour les items
    int *user_map = (int *)calloc(max_users, sizeof(int)); // Pour marquer les user_id vus
    int *item_map = (int *)calloc(max_items, sizeof(int)); // Pour marquer les item_id vus
    int local_m = 0, local_n = 0; // Compteurs d'utilisateurs et d'items distincts
    int count = 0; // Compteur de lignes
    char line[256]; // Buffer pour chaque ligne

    while (fgets(line, sizeof(line), file)) {
        count++;
        // Extraire les champs temporairement pour compter m et n
        int u_id, i_id, c_id;
        float r;
        long ts;
        if (sscanf(line, "%d%d%d%f%ld", &u_id, &i_id, &c_id, &r, &ts) != 5) {
            fprintf(stderr, "Ligne %d mal formée : %s", count, line);
            continue;
        }

        // Gérer les user_id
        while (u_id >= max_users) {
            max_users *= 2;
            user_map = (int *)realloc(user_map, max_users * sizeof(int));
            memset(user_map + max_users / 2, 0, max_users / 2 * sizeof(int));
        }
        if (!user_map[u_id]) {
            user_map[u_id] = 1;
            local_m++;
        }

        // Gérer les item_id
        while (i_id >= max_items) {
            max_items *= 2;
            item_map = (int *)realloc(item_map, max_items * sizeof(int));
            memset(item_map + max_items / 2, 0, max_items / 2 * sizeof(int));
        }
        if (!item_map[i_id]) {
            item_map[i_id] = 1;
            local_n++;
        }
    }

    // Mettre à jour les compteurs
    *num_transactions = count;
    *m = local_m;
    *n = local_n;

    // Allouer le tableau de transactions
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

    // Étape 2 : Revenir au début du fichier et remplir le tableau
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
        transactions[index].user_id = u_id;
        transactions[index].item_id = i_id;
        transactions[index].category_id = c_id;
        transactions[index].rating = r;
        transactions[index].timestamp = ts;
        index++;
    }

    // Ajuster num_transactions si des lignes ont été ignorées
    *num_transactions = index;

    // Libérer la mémoire temporaire
    free(user_map);
    free(item_map);
    fclose(file);

    // Réallouer le tableau si nécessaire (en cas de lignes ignorées)
    if (index < count) {
        transactions = (Transaction *)realloc(transactions, index * sizeof(Transaction));
    }

    return transactions;
}