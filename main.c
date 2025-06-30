#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "function_principale.h"
#include "Factorisation_matricielle/factorisation_matricielle.h"
#include "knn/knn.h"



int main() {
    int num_transactions, m, n;
    Transaction *transactions = load_transactions("ratings.txt", &num_transactions, &m, &n);
    if (!transactions) {
        printf("Échec du chargement.\n");
        return 1;
    }

    int i;
    for (i = 0; i < num_transactions; i++)
    {
        printf("Transaction %d: User %d, Item %d, Rating %.2f, Category %d, Timestamp %ld\n",
               i + 1, transactions[i].user_id, transactions[i].item_id,
               transactions[i].rating, transactions[i].category_id, transactions[i].timestamp); 
    }
    printf("Nombre de transactions : %d, Nombre d'utilisateurs : %d, Nombre d'items : %d\n\n",
           num_transactions, m, n);
    

    // Générer la matrice à partir des transactions
    float **matrix = Generate_matrix_knn(transactions, num_transactions, m, n);
    if (!matrix) {
        printf("Échec de la génération de la matrice.\n");
        free(transactions);
        return 1;
    }

    // Afficher la matrice générée
    printf("Matrice générée :\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("%.2f ", matrix[i][j]);
        }
        printf("\n");
    }

    float **knn_matrix = Pearson(matrix, m, n);
    if (!knn_matrix) {
        printf("Échec du calcul de la matrice KNN.\n"); 
        free(transactions);
        return 1;
    }
    // Afficher la matrice KNN
    printf("\nMatrice KNN :\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            printf("%.2f ", knn_matrix[i][j]);
        }
        printf("\n");
    }   

    // Exemple de prédiction

    int user_id = 0; // Utilisateur pour lequel on veut prédire la note
    int item_id = 1; // Item pour lequel on veut prédire la note

    float prediction = Predict(user_id, item_id, matrix, m, n);
    printf("\nPrédiction pour l'utilisateur %d et l'item %d : %.2f\n", user_id, item_id, prediction);
    // Exemple de prédiction pour tous les utilisateurs et items

    float **predictions = Predict_all(matrix, matrix, m, n);
    if (!predictions) {
        printf("Échec de la prédiction pour tous les utilisateurs et items.\n");
        for (int i = 0; i < m; i++) {
            free(knn_matrix[i]);
        }
        free(knn_matrix);
        for (int i = 0; i < m; i++) {
            free(matrix[i]);
        }
        free(matrix);
        free(transactions);
        return 1;
    }

    // Afficher les prédictions
    printf("\nPrédictions pour tous les utilisateurs et items :\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("%.2f ", predictions[i][j]);
        }
        printf("\n");
    }

    //Calculer la précision
    float Error_value = Erreur(matrix, matrix, m, n);
    printf("\nErreur : %.2f\n", Error_value);

    // Libérer la mémoire de la matrice KNN
    for (int i = 0; i < m; i++) {
        free(knn_matrix[i]);
    }
    free(knn_matrix); 

    // Libérer la mémoire de la matrice
    for (int i = 0; i < m; i++) {
        free(matrix[i]);
    }
    free(matrix);

    int train_num_transactions, train_m, train_n;
    Transaction *train_data = load_transactions("train.txt", &train_num_transactions, &train_m, &train_n);
    if (!train_data) {
        fprintf(stderr, "Échec du chargement des données d'entraînement\n");
        return 1;
    }

    int test_num_transactions, test_m, test_n;
    Transaction *test_data = load_transactions("test.txt", &test_num_transactions, &test_m, &test_n);
    if (!test_data) {
        fprintf(stderr, "Échec du chargement des données de test\n");
        free(train_data);
        return 1;
    }

    int m = (train_m > test_m) ? train_m : test_m;
    int n = (train_n > test_n) ? train_n : test_n;
    MF_Params params = {0.01, 0.1, 100};
    int k = 10;

    float **full_matrix = MF(train_data, train_num_transactions, &params, m, n, k);
    if (!full_matrix) {
        fprintf(stderr, "Échec de MF\n");
        free(train_data);
        free(test_data);
        return 1;
    }

    float *predictions = (float *)malloc(test_num_transactions * sizeof(float));
    if (!predictions) {
        fprintf(stderr, "Erreur d'allocation pour predictions\n");
        free_full_matrix(full_matrix, m);
        free(train_data);
        free(test_data);
        return 1;
    }
    predict_all_mf(full_matrix, test_data, test_num_transactions, predictions, m, n);

    float rmse, mae;
    evaluate_mf(test_data, test_num_transactions, predictions, &rmse, &mae);
    printf("RMSE: %f, MAE: %f\n", rmse, mae);


    free(predictions);
    free_full_matrix(full_matrix, m);
    free(train_data);
    free(test_data);
    return 0;
}