#ifndef _INCLUDE_FUNCTION_PRINCIPALE_H
#define _INCLUDE_FUNCTION_PRINCIPALE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
    int user_id;     // Identifiant de l'utilisateur
    int item_id;     // Identifiant de l'item
    float rating;    // Note donnée
    int category_id; // Catégorie (optionnel, selon usage)
    long timestamp;  // Horodatage
} Transaction;


//Ici on mettra les fonctions qui ne sont pas liees a une bibliotheque particulière comme par exemple les fonctions de manipulation de fichiers.
Transaction *load_transactions(const char *filename, int *num_transactions, int *m, int *n);
#endif