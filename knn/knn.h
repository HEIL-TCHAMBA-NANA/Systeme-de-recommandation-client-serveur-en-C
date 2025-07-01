#ifndef KNN_H
#define KNN_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../structures/struct_transaction.h"

/*******************************************************/

float **Generate_matrix_knn(Transaction *transactions, int num_transactions, int m, int n);

// Structure pour stocker l'indice et la distance (utiliser pour selectionner les K plus proches voisins)
typedef struct {
    int index;
    double distance;
} Distance;

// Fonction de tri par insertion
void insertionSort(Distance *arr, int n);
// Fonction pour trouver les K indices les plus proches
int* findKClosestIndices(float *arr, int n, int i, int k);

/******************************************************/


float moy(float * tab, int taille);

// Fonctions principale
float ** Pearson(float ** train_data, int nb_user, int nb_item);
float Predict(int user_id, int item_id, float **train_data, int nb_user, int nb_item);
float **Predict_all(float **test_data, float **train_data, int nb_user, int nb_item);

float Erreur(float **test_data, float **train_data, int nb_user, int nb_item);

#endif