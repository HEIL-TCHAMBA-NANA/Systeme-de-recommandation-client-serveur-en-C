#ifndef KNN_H
#define KNN_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Structure pour stocker l'indice et la distance
typedef struct {
    int index;
    double distance;
} Distance;

// Fonction de tri par insertion
void insertionSort(Distance *arr, int n);
// Fonction pour trouver les K indices les plus proches
int* findKClosestIndices(float *arr, int n, int i, int k);

float moy(float * tab);

// Fonctions principale
float ** Pearson(float ** train_data);
float Predict(int user_id, int item_id, float **train_data);
float **Predict_all(float **test_data, float **train_data);

float precision(float **test_data, float **train_data);

#endif