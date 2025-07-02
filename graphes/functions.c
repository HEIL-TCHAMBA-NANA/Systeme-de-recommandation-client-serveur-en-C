#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include"functions.h"

// Fonction pour charger les transactions
Transaction *load_transactions(const char *filename, int *num_transactions) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        return NULL;
    }

    int count = 0;
    char line[256];
    while (fgets(line, sizeof(line), file)) count++;
    rewind(file);

    Transaction *transactions = (Transaction *)malloc(count * sizeof(Transaction));
    if (!transactions) {
        perror("Erreur d'allocation mémoire");
        fclose(file);
        return NULL;
    }

    int index = 0;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d %d %d %f %ld", 
                   &transactions[index].user_id,
                   &transactions[index].item_id,
                   &transactions[index].category_id,
                   &transactions[index].rating,
                   &transactions[index].timestamp) == 5) {
            index++;
        }
    }

    *num_transactions = index;
    fclose(file);
    return transactions;
}

// Fonction pour obtenir les utilisateurs et items uniques
Data get_unique_users_and_items(Transaction *transactions, int num_transactions) {
    Data data = {0};
    int max_users = 100, max_items = 100;
    data.user_ids = malloc(max_users * sizeof(int));
    data.item_ids = malloc(max_items * sizeof(int));
    
    data.nbreUsers = 0;
    for (int i = 0; i < num_transactions; i++) {
        bool found = false;
        for (int j = 0; j < data.nbreUsers; j++) {
            if (data.user_ids[j] == transactions[i].user_id) {
                found = true;
                break;
            }
        }
        if (!found) {
            if (data.nbreUsers >= max_users) {
                max_users *= 2;
                data.user_ids = realloc(data.user_ids, max_users * sizeof(int));
            }
            data.user_ids[data.nbreUsers++] = transactions[i].user_id;
        }
    }
    
    data.nbreItems = 0;
    for (int i = 0; i < num_transactions; i++) {
        bool found = false;
        for (int j = 0; j < data.nbreItems; j++) {
            if (data.item_ids[j] == transactions[i].item_id) {
                found = true;
                break;
            }
        }
        if (!found) {
            if (data.nbreItems >= max_items) {
                max_items *= 2;
                data.item_ids = realloc(data.item_ids, max_items * sizeof(int));
            }
            data.item_ids[data.nbreItems++] = transactions[i].item_id;
        }
    }
    
    return data;
}

// Fonction pour créer la matrice d'adjacence pondérée
float** create_weighted_adjacency_matrix(Transaction *transactions, int num_transactions, Data data) {
    int size = data.nbreUsers + data.nbreItems;
    float **matrix = malloc(size * sizeof(float *));
    
    for (int i = 0; i < size; i++) {
        matrix[i] = calloc(size, sizeof(float));
    }
    
    for (int i = 0; i < num_transactions; i++) {
        int user_idx = -1, item_idx = -1;
        
        for (int j = 0; j < data.nbreUsers; j++) {
            if (data.user_ids[j] == transactions[i].user_id) {
                user_idx = j;
                break;
            }
        }
        
        for (int j = 0; j < data.nbreItems; j++) {
            if (data.item_ids[j] == transactions[i].item_id) {
                item_idx = data.nbreUsers + j;
                break;
            }
        }
        
        if (user_idx != -1 && item_idx != -1) {
            float normalized_rating = transactions[i].rating / 5.0;
            matrix[user_idx][item_idx] = normalized_rating;
            matrix[item_idx][user_idx] = normalized_rating;
        }
    }
    
    return matrix;
}

// Normalisation de la matrice
void normalize_matrix(float **matrix, int size) {
    for (int i = 0; i < size; i++) {
        float sum = 0.0;
        for (int j = 0; j < size; j++) {
            sum += matrix[i][j];
        }
        if (sum > 0) {
            for (int j = 0; j < size; j++) {
                matrix[i][j] /= sum;
            }
        }
    }
}

// Algorithme PageRank personnalisé
float* personalized_pagerank(float **matrix, float *personalization, int size, float alpha, int max_iter) {
    float *pr = malloc(size * sizeof(float));
    float *new_pr = malloc(size * sizeof(float));
    
    for (int i = 0; i < size; i++) {
        pr[i] = 1.0 / size;
    }
    
    for (int iter = 0; iter < max_iter; iter++) {
        for (int i = 0; i < size; i++) {
            new_pr[i] = 0.0;
            for (int j = 0; j < size; j++) {
                new_pr[i] += matrix[j][i] * pr[j];
            }
            new_pr[i] = alpha * new_pr[i] + (1 - alpha) * personalization[i];
        }
        
        float sum = 0.0;
        for (int i = 0; i < size; i++) sum += new_pr[i];
        for (int i = 0; i < size; i++) new_pr[i] /= sum;
        
        memcpy(pr, new_pr, size * sizeof(float));
    }
    
    free(new_pr);
    return pr;
}

// Fonction pour créer la matrice de prédictions
float** create_prediction_matrix(Data data, float **adj_matrix, float alpha) {
    float **prediction_matrix = malloc(data.nbreUsers * sizeof(float *));
    
    for (int i = 0; i < data.nbreUsers; i++) {
        prediction_matrix[i] = malloc(data.nbreItems * sizeof(float));
        
        // Créer le vecteur de personnalisation pour cet utilisateur
        float *personalization = calloc(data.nbreUsers + data.nbreItems, sizeof(float));
        personalization[i] = 1.0;
        
        // Calculer le PageRank personnalisé
        float *pagerank = personalized_pagerank(adj_matrix, personalization, 
                                              data.nbreUsers + data.nbreItems, alpha, 100);
        
        // Remplir la ligne de la matrice de prédiction avec les scores des items
        for (int j = 0; j < data.nbreItems; j++) {
            // Dénormaliser le score (ramener à l'échelle 0-5)
            prediction_matrix[i][j] = pagerank[data.nbreUsers + j] * 5.0;
        }
        
        free(personalization);
        free(pagerank);
    }
    
    return prediction_matrix;
}

// Fonction pour afficher la matrice de prédictions
void print_prediction_matrix(float **matrix, Data data) {
    printf("\nMatrice de prédictions (utilisateurs x items):\n\n");
    
    // En-tête des colonnes (items)
    printf("%10s", "User/Item");
    for (int j = 0; j < data.nbreItems; j++) {
        printf("%8d", data.item_ids[j]);
    }
    printf("\n");
    
    // Lignes de la matrice
    for (int i = 0; i < data.nbreUsers; i++) {
        printf("%8d |", data.user_ids[i]);
        for (int j = 0; j < data.nbreItems; j++) {
            printf("%8.2f", matrix[i][j]);
        }
        printf("\n");
    }
}

float ** get_matrice_predictions(const char *filename, int num_transactions, Transaction *transactions,Data data, float **adj_matrix, float alpha, float **prediction_matrix) {
   
    // 1. Charger les données
    transactions = load_transactions(filename, &num_transactions);
    if (!transactions || num_transactions == 0) {
        printf("Erreur lors du chargement des données.\n");
        return 1;
    }
    
    // 2. Obtenir les utilisateurs et items uniques
    data = get_unique_users_and_items(transactions, num_transactions);
    
    // 3. Créer la matrice d'adjacence pondérée
    adj_matrix = create_weighted_adjacency_matrix(transactions, num_transactions, data);
    
    // 4. Normaliser la matrice
    normalize_matrix(adj_matrix, data.nbreUsers + data.nbreItems);
    
    // 5. Créer la matrice de prédictions
    prediction_matrix = create_prediction_matrix(data, adj_matrix, alpha);

    
    return prediction_matrix;
}