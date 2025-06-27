#include "knn.h"

float moy(float * tab) {
    float somme = 0;
    int taille  = sizeof(tab) / sizeof(tab[0]);
    int i, non_null = 0; // Nbre de donées différents de 0

    for (i = 0; i < taille; i++)
    {
        if (tab[i] != 0)
        {
            non_null ++;
        }   
        somme += tab[i];
    }

    return somme / taille;
}

float **Pearson(float ** train_data) {
    float **result = malloc(2 * sizeof(float));
    
    int i,j,k;
    int nb_user = sizeof(train_data) / sizeof(train_data[0]);
    int nb_item = sizeof(train_data[0]) / sizeof(train_data[0][0]);

    // Calcul des moyennes pour chaque utilisateur
    float *moyennes = malloc(nb_user * sizeof(float));
    
    for (i = 0; i < nb_user; i++) {
        moyennes[i] = moy(train_data[i]);
    }

    float *rac = malloc(nb_user * sizeof(float)); // Tableau contenant la racine de chaque user_id

    for (i = 0; i < nb_user; i++)
    {
        rac[i] = 0; // Initialisation du tableau
    }


    for ( i = 0; i < nb_user; i++)
    {
        for ( j = 0; i < nb_user; i++)
        {
            result[i][j] = 0; // Initialisation de la matrice de résultats

            for ( k = 0; i < nb_item; i++)
            {
                if (train_data[i][k] != 0 && train_data[j][k] != 0) {
                    result[i][j] += (train_data[i][k] - moyennes[i]) * (train_data[j][k] - moyennes[j]);
                    rac[i] += (train_data[i][k] - moyennes[i])*(train_data[i][k] - moyennes[i]);
                    rac[j] += (train_data[j][k] - moyennes[j])*(train_data[j][k] - moyennes[j]);
                }
            }
            result[i][j] = result[i][j] / (sqrt(rac[i])*sqrt(rac[j]));  
        }
        
    }

    return result;
}


float Predict(int user_id, int item_id, float **train_data) {
    float **result = malloc(2 * sizeof(float));

    result = Pearson(train_data);
    int n = sizeof(result) / sizeof(result[0]); // Nombre d'utilisateurs 
    
    int i, j = 0;
    float min[n]; // Tableau des voisins (indices, valeurs)

   
    
    for(i = 0; i<n; i++){
        min[j] = fabs(result[user_id][user_id] - result[user_id][i]);
        j++;
    }

    // On va considérer k = 2 (ie 2-NN)
    int k = 2; // Nombre de voisins à considérer
    int * KNN = findKClosestIndices(min, n, user_id, k); // On cherche les 2 plus proches voisins

    float prediction = 0.0;
    float somme = 0.0;

    for (i = 0; i < k; i++)
    {
        prediction += train_data[KNN[i]][item_id] * result[user_id][KNN[i]];
        somme += result[user_id][KNN[i]];
    }

    prediction = somme != 0 ? prediction / somme : 0.0; // Si la somme est nulle, on reste avec 1
    
    return prediction < 1 ? 1.0 : prediction; // On ne peut pas prédire une note inférieure à 1
}

void insertionSort(Distance *arr, int n) {
    for (int i = 1; i < n; i++) {
        Distance key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].distance > key.distance) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

int* findKClosestIndices(float *arr, int n, int i, int k) { // n = taille du tableau, i = indice de base, k = nombres de voisins à retourner
    // Vérification des paramètres
    if (i < 0 || i >= n || k > n || k <= 0) {
        return NULL;
    }

    // Allocation dynamique du tableau de distances
    Distance *distances = (Distance *)malloc(n * sizeof(Distance));
    if (distances == NULL) {
        return NULL;
    }

    // Allocation dynamique du tableau de résultat
    int *result = (int *)malloc(k * sizeof(int));
    if (result == NULL) {
        free(distances);
        return NULL;
    }

    // Calculer les distances absolues
    float target = arr[i];
    for (int j = 0; j < n; j++) {
        distances[j].index = j;
        distances[j].distance = fabs(arr[j] - target);
    }

    // Trier les distances avec le tri par insertion
    insertionSort(distances, n);

    // Remplir le tableau de résultat avec les K premiers indices sans considérer le premier élément (lui-même)
    for (int j = 1; j < k+1; j++) {
        result[j] = distances[j].index;
    }

    // Libérer la mémoire du tableau de distances
    free(distances);

    return result;
}

float **Predict_all(float **test_data, float **train_data){
    int i,j;

    float **result ;
    
    int user_id, item_id;
    int nb_user = sizeof(test_data) / sizeof(test_data[0]);
    int nb_item = sizeof(test_data[0]) / sizeof(test_data[0][0]);
    

    // Initialisation de la matrice de résultats
    result = malloc(nb_user * sizeof(float *)); 
    for (i = 0; i < nb_user; i++) {
        result[i] = malloc(nb_item * sizeof(float));
        for (j = 0; j < nb_item; j++) {
            result[i][j] = 0.0; // Initialisation des notes prédites à 0
        }
    }

    // On va prédire les notes pour chaque utilisateur et chaque item
    for (i = 0; i < nb_user; i++)
    {
        for (j = 0; j < nb_item; j++)
        {
            if (test_data[i][j] != 0) { // Si la note est présente dans le test_data, on la predit pour tester la précision
                user_id = i;
                item_id = j;
                result[i][j] = Predict(user_id, item_id, train_data);
            }
        }
    }

    return result; // On retourne la matrice de résultats et la précision obtenue
}

float precision(float **test_data, float **train_data) { // Rooute Mean Square Error (RMSE)
    // On va calculer la racine carrée de la moyenne des différences carrées entre les notes réelles et les notes prédites
    int i, j;
    float precision = 0.0;
    int k = 0; // Compteur pour les notes prédites
    int nb_user = sizeof(test_data) / sizeof(test_data[0]);
    int nb_item = sizeof(test_data[0]) / sizeof(test_data[0][0]);

    for (i = 0; i < nb_user; i++) {
        for (j = 0; j < nb_item; j++) {
            if (test_data[i][j] != 0) { // Si la note est présente dans le test_data, on la predit pour tester la précision
                k++;
                precision += (test_data[i][j] - Predict(i, j, train_data)) * (test_data[i][j] - Predict(i, j, train_data)); // On ajoute la différence carrée entre la note réelle et la note prédite
            }
        }
    }

    precision = sqrt(precision / k); // On calcule la racine carrée de la moyenne des différences carrées

    return precision;
}

