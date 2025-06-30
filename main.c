#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "function_principale.h"
#include "Factorisation_matricielle/factorisation_matricielle.h"
#include "struct_transaction.h"



int main() {
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