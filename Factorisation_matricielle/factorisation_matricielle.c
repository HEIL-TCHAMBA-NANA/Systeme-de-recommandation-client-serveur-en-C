
#include "factorisation_matricielle.h"
#include "function_principale.h"



void initialize_mf_model(MF_Model *model, int m, int n, int k) {
    // Ajout 1 : Vérification du pointeur model
    if (!model) {
        fprintf(stderr, "Pointeur model nul\n");
        return;
    }
    // Ajout 2 : Vérification des dimensions
    if (m <= 0 || n <= 0 || k <= 0) {
        fprintf(stderr, "Dimensions invalides : m=%d, n=%d, k=%d\n", m, n, k);
        return;
    }
    // Ajout 3 : Initialisation de la graine pour rand()
    static int seed_initialized = 0;
    if (!seed_initialized) {
        srand(time(NULL));
        seed_initialized = 1;
    }

    model->m = m;
    model->n = n;
    model->k = k;

    // Allouer U (m x k)
    model->U = (float **)malloc(m * sizeof(float *));
    // Ajout 4 : Vérification de l'allocation
    if (!model->U) {
        fprintf(stderr, "Erreur d'allocation pour U\n");
        return;
    }
    for (int i = 0; i < m; i++) {
        model->U[i] = (float *)malloc(k * sizeof(float));
        // Ajout 5 : Vérification de l'allocation
        if (!model->U[i]) {
            fprintf(stderr, "Erreur d'allocation pour U[%d]\n", i);
            // Libérer la mémoire allouée partiellement
            for (int j = 0; j < i; j++) free(model->U[j]);
            free(model->U);
            model->U = NULL;
            return;
        }
        for (int s = 0; s < k; s++) {
            model->U[i][s] = ((float)rand() / RAND_MAX) * 0.1;
        }
    }

    // Allouer V (n x k)
    model->V = (float **)malloc(n * sizeof(float *));
    // Ajout 6 : Vérification de l'allocation
    if (!model->V) {
        fprintf(stderr, "Erreur d'allocation pour V\n");
        // Libérer U
        for (int i = 0; i < m; i++) free(model->U[i]);
        free(model->U);
        model->U = NULL;
        return;
    }
    for (int j = 0; j < n; j++) {
        model->V[j] = (float *)malloc(k * sizeof(float));
        // Ajout 7 : Vérification de l'allocation
        if (!model->V[j]) {
            fprintf(stderr, "Erreur d'allocation pour V[%d]\n", j);
            // Libérer U et V partiellement
            for (int i = 0; i < m; i++) free(model->U[i]);
            free(model->U);
            model->U = NULL;
            for (int k = 0; k < j; k++) free(model->V[k]);
            free(model->V);
            model->V = NULL;
            return;
        }
        for (int s = 0; s < k; s++) {
            model->V[j][s] = ((float)rand() / RAND_MAX) * 0.1;
        }
    }

    // Allauer O (biais utilisateurs)
    model->O = (float *)calloc(m, sizeof(float));
    // Ajout 8 : Vérification de l'allocation
    if (!model->O) {
        fprintf(stderr, "Erreur d'allocation pour O\n");
        // Libérer U et V
        for (int i = 0; i < m; i++) free(model->U[i]);
        free(model->U);
        model->U = NULL;
        for (int j = 0; j < n; j++) free(model->V[j]);
        free(model->V);
        model->V = NULL;
        return;
    }

    // Allouer P (biais items)
    model->P = (float *)calloc(n, sizeof(float));
    // Ajout 9 : Vérification de l'allocation
    if (!model->P) {
        fprintf(stderr, "Erreur d'allocation pour P\n");
        // Libérer U, V, O
        for (int i = 0; i < m; i++) free(model->U[i]);
        free(model->U);
        model->U = NULL;
        for (int j = 0; j < n; j++) free(model->V[j]);
        free(model->V);
        model->V = NULL;
        free(model->O);
        model->O = NULL;
        return;
    }
}


// Fonction pour calculer la perte J
float compute_loss(MF_Model *model, Transaction *transactions, int num_transactions, float lambda) {
    // Ajout 1 : Vérification des pointeurs
    if (!model || !transactions) {
        fprintf(stderr, "Pointeur nul\n");
        return 0.0;
    }
    // Ajout 2 : Vérification de num_transactions
    if (num_transactions <= 0) {
        fprintf(stderr, "num_transactions invalide\n");
        return 0.0;
    }

    float loss = 0.0;
    for (int t = 0; t < num_transactions; t++) {
        int i = transactions[t].user_id;
        int j = transactions[t].item_id;
        // Ajout 3 : Vérification des indices
        if (i >= model->m || j >= model->n || i < 0 || j < 0) {
            fprintf(stderr, "Indice invalide : user_id=%d, item_id=%d\n", i, j);
            continue;
        }
        float r_ij = transactions[t].rating;

        float r_hat = model->O[i] + model->P[j];
        for (int s = 0; s < model->k; s++) {
            r_hat += model->U[i][s] * model->V[j][s];
        }
        float error = r_ij - r_hat;
        loss += error * error;
    }
    loss *= 0.5;

    float reg_term = 0.0;
    for (int i = 0; i < model->m; i++) {
        for (int s = 0; s < model->k; s++) {
            reg_term += model->U[i][s] * model->U[i][s];
        }
        reg_term += model->O[i] * model->O[i];
    }
    for (int j = 0; j < model->n; j++) {
        for (int s = 0; s < model->k; s++) {
            reg_term += model->V[j][s] * model->V[j][s];
        }
        reg_term += model->P[j] * model->P[j];
    }
    loss += (lambda * 0.5) * reg_term;

    return loss;
}


void train_mf_model(MF_Model *model, Transaction *transactions, int num_transactions, MF_Params *params) {
    // Ajout 1 : Vérification des pointeurs
    if (!model || !transactions || !params) {
        fprintf(stderr, "Pointeur nul\n");
        return;
    }
    // Ajout 2 : Vérification de num_transactions
    if (num_transactions <= 0) {
        fprintf(stderr, "num_transactions invalide\n");
        return;
    }

    int m = model->m, n = model->n, k = model->k;
    float alpha = params->alpha, lambda = params->lambda;
    int num_epochs = params->num_epochs;

    for (int epoch = 0; epoch < num_epochs; epoch++) {
        for (int t = 0; t < num_transactions; t++) {
            int i = transactions[t].user_id;
            int j = transactions[t].item_id;
            // Ajout 3 : Vérification des indices
            if (i >= m || j >= n || i < 0 || j < 0) {
                fprintf(stderr, "Indice invalide : user_id=%d, item_id=%d\n", i, j);
                continue;
            }
            float r_ij = transactions[t].rating;

            float r_hat = model->O[i] + model->P[j];
            for (int s = 0; s < k; s++) {
                r_hat += model->U[i][s] * model->V[j][s];
            }

            float error = r_ij - r_hat;

            model->O[i] += alpha * (error - lambda * model->O[i]);
            model->P[j] += alpha * (error - lambda * model->P[j]);

            for (int s = 0; s < k; s++) {
                float u_is = model->U[i][s];
                float v_js = model->V[j][s];
                model->U[i][s] += alpha * (error * v_js - lambda * u_is);
                model->V[j][s] += alpha * (error * u_is - lambda * v_js);
            }
        }
        float loss = compute_loss(model, transactions, num_transactions, lambda);
        printf("Epoch %d, Loss: %f\n", epoch, loss);
    }
}




float predict_mf(MF_Model *model, int user_id, int item_id) {
    // Ajout 1 : Vérification du pointeur
    if (!model) {
        fprintf(stderr, "Pointeur model nul\n");
        return 0.0;
    }
    // Ajout 2 : Vérification des indices
    if (user_id >= model->m || item_id >= model->n || user_id < 0 || item_id < 0) {
        fprintf(stderr, "Indice invalide : user_id=%d, item_id=%d\n", user_id, item_id);
        return 0.0;
    }

    float r_hat = model->O[user_id] + model->P[item_id];
    for (int s = 0; s < model->k; s++) {
        r_hat += model->U[user_id][s] * model->V[item_id][s];
    }
    return r_hat;
}



float **generate_full_matrix(MF_Model *model) {
    // Ajout 1 : Vérification du pointeur
    if (!model) {
        fprintf(stderr, "Pointeur model nul\n");
        return NULL;
    }
    // Ajout 2 : Vérification des dimensions
    if (model->m <= 0 || model->n <= 0) {
        fprintf(stderr, "Dimensions invalides : m=%d, n=%d\n", model->m, model->n);
        return NULL;
    }

    float **full_matrix = (float **)malloc(model->m * sizeof(float *));
    // Ajout 3 : Vérification de l'allocation
    if (!full_matrix) {
        fprintf(stderr, "Erreur d'allocation pour full_matrix\n");
        return NULL;
    }
    for (int i = 0; i < model->m; i++) {
        full_matrix[i] = (float *)malloc(model->n * sizeof(float));
        // Ajout 4 : Vérification de l'allocation
        if (!full_matrix[i]) {
            fprintf(stderr, "Erreur d'allocation pour full_matrix[%d]\n", i);
            for (int j = 0; j < i; j++) free(full_matrix[j]);
            free(full_matrix);
            return NULL;
        }
        for (int j = 0; j < model->n; j++) {
            full_matrix[i][j] = predict_mf(model, i, j);
        }
    }
    return full_matrix;
}


void predict_all_mf(float **full_matrix, Transaction *test_data, int num_test, float *predictions, int m, int n) {
    // Ajout 1 : Vérification des pointeurs
    if (!full_matrix || !test_data || !predictions) {
        fprintf(stderr, "Pointeur nul\n");
        return;
    }
    // Ajout 2 : Vérification de num_test
    if (num_test <= 0) {
        fprintf(stderr, "num_test invalide\n");
        return;
    }
    // Ajout 3 : Vérification des dimensions
    if (m <= 0 || n <= 0) {
        fprintf(stderr, "Dimensions invalides : m=%d, n=%d\n", m, n);
        return;
    }

    for (int t = 0; t < num_test; t++) {
        int i = test_data[t].user_id;
        int j = test_data[t].item_id;
        // Ajout 4 : Vérification des indices
        if (i >= m || j >= n || i < 0 || j < 0) {
            fprintf(stderr, "Indice invalide : user_id=%d, item_id=%d\n", i, j);
            continue;
        }
        predictions[t] = full_matrix[i][j];
    }
}


void evaluate_mf(Transaction *test_data, int num_test, float *predictions, float *rmse, float *mae) {
    // Ajout 1 : Vérification des pointeurs
    if (!test_data || !predictions || !rmse || !mae) {
        fprintf(stderr, "Pointeur nul\n");
        return;
    }
    // Ajout 2 : Vérification de num_test
    if (num_test <= 0) {
        fprintf(stderr, "num_test invalide\n");
        *rmse = *mae = 0.0;
        return;
    }

    float sum_se = 0, sum_ae = 0;
    for (int t = 0; t < num_test; t++) {
        float error = test_data[t].rating - predictions[t];
        sum_se += error * error;
        sum_ae += fabs(error);
    }
    *rmse = sqrt(sum_se / num_test);
    *mae = sum_ae / num_test;
}




void free_mf_model(MF_Model *model) {
    // Vérification du pointeur
    if (!model) {
        fprintf(stderr, "Pointeur model nul\n");
        return;
    }
    // Libérer U
    if (model->U) {
        for (int i = 0; i < model->m; i++) {
            if (model->U[i]) free(model->U[i]);
        }
        free(model->U);
    }
    // Libérer V
    if (model->V) {
        for (int j = 0; j < model->n; j++) {
            if (model->V[j]) free(model->V[j]);
        }
        free(model->V);
    }
    // Libérer O
    if (model->O) free(model->O);
    // Libérer P
    if (model->P) free(model->P);
    // Réinitialiser les pointeurs
    model->U = NULL;
    model->V = NULL;
    model->O = NULL;
    model->P = NULL;
}


void free_full_matrix(float **matrix, int m) {
    // Vérification du pointeur et de la dimension
    if (!matrix || m <= 0) {
        fprintf(stderr, "Pointeur matrix nul ou m invalide\n");
        return;
    }
    for (int i = 0; i < m; i++) {
        if (matrix[i]) free(matrix[i]);
    }
    free(matrix);
}