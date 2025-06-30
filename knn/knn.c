#include "knn.h"

float moy(float * tab, int taille) {
    float somme = 0;
    int i, non_null = 0; // Nbre de donées différents de 0

    for (i = 0; i < taille; i++)
    {
        if (tab[i] != 0)
        {
            non_null ++;
        }   
        somme += tab[i];
    }

    return somme / non_null;
}

float **Pearson(float ** train_data, int nb_user, int nb_item) {

    int i,j,k;

    float **result = malloc(nb_user * sizeof(float));
    for (i = 0; i < nb_user; i++) {
        result[i] = malloc(nb_user * sizeof(float)); // Initialisation de la matrice de résultats
    }

    // Calcul des moyennes pour chaque utilisateur
    float *moyennes = malloc(nb_user * sizeof(float));
    
    for (i = 0; i < nb_user; i++) {
        moyennes[i] = moy(train_data[i], nb_item);
    }
    
    float rac_i, rac_j;

    for ( i = 0; i < nb_user; i++)
    {
        for ( j = 0; j < nb_user; j++)
        {
            rac_i = 0.0; // Initialisation de la racine de l'utilisateur i
            rac_j = 0.0; // Initialisation de la racine de l'utilisateur j
            
            result[i][j] = 0.0; // Initialisation de la matrice de résultats

            for ( k = 0; k < nb_item; k++)
            {
                if (train_data[i][k] != 0 && train_data[j][k] != 0) {
                    result[i][j] += (train_data[i][k] - moyennes[i]) * (train_data[j][k] - moyennes[j]);
                    rac_i += (train_data[i][k] - moyennes[i])*(train_data[i][k] - moyennes[i]);
                    rac_j += (train_data[j][k] - moyennes[j])*(train_data[j][k] - moyennes[j]);

                    //printf("train_data[%d][%d] = %.2f, train_data[%d][%d] = %.2f, result[%d][%d] = %.2f, rac[%d] = %.2f, rac[%d] = %.2f\n", 
                    //       i, k, train_data[i][k], j, k, train_data[j][k], i, j, result[i][j], i, rac_i, j, rac_j);
                }
            }
            result[i][j] = result[i][j] / (sqrtf(rac_i)*sqrtf(rac_j));  

            if (i == j) {
                result[i][j] = 1.0; // La similarité d'un utilisateur avec lui-même est toujours 1
            }
            else if (isnan(result[i][j]) || isinf(result[i][j]))  // Vérification si la valeur est NaN ou infinie
            {
                result[i][j] = 0.0; // Si la valeur est NaN (division par zero), on la remplace par 0
            }   
        }
    }
    return result;
}


float Predict(int user_id, int item_id, float **train_data, int nb_user, int nb_item) {
    
    int i, j = 0;

    float **result = malloc(nb_user * sizeof(float));
    for (i = 0; i < nb_user; i++) {
        result[i] = malloc(nb_user * sizeof(float)); // Initialisation de la matrice de résultats
    }

    result = Pearson(train_data, nb_user, nb_item); // On calcule la matrice de similarité Pearson

    //calcul des moyennes pour chaque utilisateur
    float *moyennes = malloc(nb_user * sizeof(float));
    
    for (i = 0; i < nb_user; i++) {
        moyennes[i] = moy(train_data[i], nb_item);
    }

    int n = nb_user; // Nombre d'utilisateurs

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
        prediction += (train_data[KNN[i]][item_id]-moyennes[KNN[i]]) * result[user_id][KNN[i]];
        somme += result[user_id][KNN[i]];
    }

    prediction = somme != 0 ? prediction / somme : 0.0; // Si la somme est nulle, on reste avec 0.0
    
    return (prediction + moyennes[user_id]) < 1 ? 1.0 : (prediction +moyennes[user_id]); // On ne peut pas prédire une note inférieure à 1
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
    
    int j;

    for ( j = 0; j < k; j++)
    {
        result[j] = -1; // Initialisation du tableau de résultat avec -1
    }
    

    // Calculer les distances absolues
    float target = arr[i];
    for (int j = 0; j < n; j++) {
        distances[j].index = j;
        distances[j].distance = arr[j];
    }

    // Trier les distances avec le tri par insertion
    insertionSort(distances, n);
    
    // Remplir le tableau de résultat avec les K premiers indices sans considérer le premier élément (lui-même)
    int count = 0;
    for (int j = 1; j < k+1; j++) {
        result[count] = distances[j].index;
        count++;
    }
    
    // Libérer la mémoire du tableau de distances
    free(distances);
    
    return result;
}

float **Predict_all(float **test_data, float **train_data, int nb_user, int nb_item) {
    int i,j;

    float **result ;
    
    int user_id, item_id;
    

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
                result[i][j] = Predict(user_id, item_id, train_data, nb_user, nb_item); // On prédit la note pour l'utilisateur i et l'item j
            }
        }
    }

    return result; // On retourne la matrice de résultats et la précision obtenue
}


float Erreur(float **test_data, float **train_data, int nb_user, int nb_item) { // Rooute Mean Square Error (RMSE)
    // On va calculer la racine carrée de la moyenne des différences carrées entre les notes réelles et les notes prédites
    int i, j;
    float precision = 0.0;
    int k = 0; // Compteur pour les notes prédites

    for (i = 0; i < nb_user; i++) {
        for (j = 0; j < nb_item; j++) {
            if (test_data[i][j] != 0) { // Si la note est présente(est non nulle) dans le test_data, on la considère pour calculer la précision
                k++;
                precision += (test_data[i][j] - Predict(i, j, train_data, nb_user, nb_item)) * (test_data[i][j] - Predict(i, j, train_data, nb_user, nb_item)); // On ajoute la différence carrée entre la note réelle et la note prédite
            }
        }
    }

    precision = sqrt(precision / k); // On calcule la racine carrée de la moyenne des différences carrées

    return precision;
}

