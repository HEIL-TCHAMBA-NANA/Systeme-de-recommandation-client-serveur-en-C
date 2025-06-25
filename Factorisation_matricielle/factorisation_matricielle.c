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