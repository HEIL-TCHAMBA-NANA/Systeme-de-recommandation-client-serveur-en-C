#ifndef STRUCT_TRANSACTION_H
#define STRUCT_TRANSACTION_H
#include <math.h>
typedef struct {
    int user_id;     // Identifiant de l'utilisateur
    int item_id;     // Identifiant de l'item
    float rating;    // Note donnée
    int category_id; // Catégorie (optionnel, selon usage)
    long timestamp;  // Horodatage
} Transaction;

#endif