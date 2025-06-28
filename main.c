#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "function_principale.h"
#include "Factorisation_matricielle/factorisation_matricielle.h"


int main() {
    int num_transactions, m, n;
    Transaction *transactions = load_transactions("test_ratings.txt", &num_transactions, &m, &n);
    if (!transactions) {
        printf("Échec du chargement.\n");
        return 1;
    }

    // Initialiser le modèle
    MF_Model model;
    initialize_mf_model(&model, m, n, 2); // k = 2 facteurs latents

    // Paramètres
    float lambda = 0.1;

    // Calculer la perte initiale
    float loss = compute_loss(&model, transactions, num_transactions, lambda);
    printf("Perte initiale : %f\n", loss);

    // Libérer la mémoire
    for (int i = 0; i < model.m; i++) free(model.U[i]);
    for (int j = 0; j < model.n; j++) free(model.V[j]);
    free(model.U);
    free(model.V);
    free(model.O);
    free(model.P);
    free(transactions);

    return 0;
}