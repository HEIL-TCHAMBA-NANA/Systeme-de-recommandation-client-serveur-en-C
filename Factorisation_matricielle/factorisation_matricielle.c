#include <stdio.h>
#include <stdlib.h>
#include "factorisation_matricielle.h"
#include "function_principale.h"






void initialize_mf_model(MF_Model *model, int m, int n, int k) {
    model->m = m;
    model->n = n;
    model->k = k;

    // Allouer U (m x k)
    model->U = (float **)malloc(m * sizeof(float *));
    for (int i = 0; i < m; i++) {
        model->U[i] = (float *)malloc(k * sizeof(float));
        for (int s = 0; s < k; s++) {
            model->U[i][s] = ((float)rand() / RAND_MAX) * 0.1; // Valeurs aléatoires entre 0 et 0.1
        }
    }

    // Allouer V (n x k)
    model->V = (float **)malloc(n * sizeof(float *));
    for (int j = 0; j < n; j++) {
        model->V[j] = (float *)malloc(k * sizeof(float));
        for (int s = 0; s < k; s++) {
            model->V[j][s] = ((float)rand() / RAND_MAX) * 0.1;
        }
    }

    // Allouer O (biais utilisateurs)
    model->O = (float *)calloc(m, sizeof(float)); // Initialisé à 0

    // Allouer P (biais items)
    model->P = (float *)calloc(n, sizeof(float)); // Initialisé à 0
}


#include <stdio.h>
#include <stdlib.h>

// Structures définies dans ton code précédent
typedef struct {
    int user_id;     // Identifiant de l'utilisateur
    int item_id;     // Identifiant de l'item
    float rating;    // Note donnée
    int category_id; // Catégorie
    long timestamp;  // Horodatage
} Transaction;

typedef struct {
    float **U;       // Matrice des facteurs latents utilisateurs (m x k)
    float **V;       // Matrice des facteurs latents items (n x k)
    float *O;        // Biais utilisateurs (taille m)
    float *P;        // Biais items (taille n)
    int m;           // Nombre d'utilisateurs
    int n;           // Nombre d'items
    int k;           // Nombre de facteurs latents
} MF_Model;

// Fonction pour calculer la perte J
float compute_loss(MF_Model *model, Transaction *transactions, int num_transactions, float lambda) {
    float loss = 0.0;

    // Étape 1 : Calculer la somme des erreurs quadratiques (1/2 * sum(r_ij - r_hat_ij)^2)
    for (int t = 0; t < num_transactions; t++) {
        int i = transactions[t].user_id;  // Utilisateur
        int j = transactions[t].item_id;  // Item
        float r_ij = transactions[t].rating; // Note réelle

        // Calculer la note prédite : r_hat = u_i . v_j + o_i + p_j
        float r_hat = model->O[i] + model->P[j];
        for (int s = 0; s < model->k; s++) {
            r_hat += model->U[i][s] * model->V[j][s];
        }

        // Ajouter l'erreur quadratique
        float error = r_ij - r_hat;
        loss += error * error;
    }
    loss *= 0.5; // Diviser par 2

    // Étape 2 : Calculer le terme de régularisation
    float reg_term = 0.0;

    // Somme des normes au carré des vecteurs U (utilisateurs)
    for (int i = 0; i < model->m; i++) {
        for (int s = 0; s < model->k; s++) {
            reg_term += model->U[i][s] * model->U[i][s];
        }
        reg_term += model->O[i] * model->O[i]; // Biais utilisateur
    }

    // Somme des normes au carré des vecteurs V (items)
    for (int j = 0; j < model->n; j++) {
        for (int s = 0; s < model->k; s++) {
            reg_term += model->V[j][s] * model->V[j][s];
        }
        reg_term += model->P[j] * model->P[j]; // Biais item
    }

    // Multiplier par lambda/2 et ajouter au coût
    loss += (lambda * 0.5) * reg_term;

    return loss;
}


void train_mf_model(MF_Model *model, Transaction *transactions, int num_transactions, MF_Params *params) {
    int m = model->m, n = model->n, k = model->k;
    float alpha = params->alpha, lambda = params->lambda;
    int num_epochs = params->num_epochs;

    for (int epoch = 0; epoch < num_epochs; epoch++) {
        for (int t = 0; t < num_transactions; t++) {
            int i = transactions[t].user_id;  // Utilisateur
            int j = transactions[t].item_id;  // Item
            float r_ij = transactions[t].rating; // Note réelle

            // Calculer la note prédite : r_hat = u_i . v_j + o_i + p_j
            float r_hat = model->O[i] + model->P[j];
            for (int s = 0; s < k; s++) {
                r_hat += model->U[i][s] * model->V[j][s];
            }

            // Calculer l'erreur
            float error = r_ij - r_hat;

            // Mettre à jour les biais
            model->O[i] += alpha * (error - lambda * model->O[i]);
            model->P[j] += alpha * (error - lambda * model->P[j]);

            // Mettre à jour U et V
            for (int s = 0; s < k; s++) {
                float u_is = model->U[i][s];
                float v_js = model->V[j][s];
                model->U[i][s] += alpha * (error * v_js - lambda * u_is);
                model->V[j][s] += alpha * (error * u_is - lambda * v_js);
            }
        }
        // Optionnel : afficher la perte J pour surveiller la convergence
        float loss = compute_loss(model, transactions, num_transactions, lambda);
        printf("Epoch %d, Loss: %f\n", epoch, loss);
    }
}




float predict_mf(MF_Model *model, int user_id, int item_id) {
    float r_hat = model->O[user_id] + model->P[item_id];
    for (int s = 0; s < model->k; s++) {
        r_hat += model->U[user_id][s] * model->V[item_id][s];
    }
    return r_hat;
}

float **generate_full_matrix(MF_Model *model) {
    float **full_matrix = (float **)malloc(model->m * sizeof(float *));
    for (int i = 0; i < model->m; i++) {
        full_matrix[i] = (float *)malloc(model->n * sizeof(float));
        for (int j = 0; j < model->n; j++) {
            full_matrix[i][j] = predict_mf(model, i, j);
        }
    }
    return full_matrix;
}

void predict_all_mf(float **full_matrix, Transaction *test_data, int num_test, float *predictions) {
    for (int t = 0; t < num_test; t++) {
        int i = test_data[t].user_id;
        int j = test_data[t].item_id;
        predictions[t] = full_matrix[i][j];
    }
}



void evaluate_mf(Transaction *test_data, int num_test, float *predictions, float *rmse, float *mae) {
    float sum_se = 0, sum_ae = 0;
    for (int t = 0; t < num_test; t++) {
        float error = test_data[t].rating - predictions[t];
        sum_se += error * error;
        sum_ae += fabs(error);
    }
    *rmse = sqrt(sum_se / num_test);
    *mae = sum_ae / num_test;
}