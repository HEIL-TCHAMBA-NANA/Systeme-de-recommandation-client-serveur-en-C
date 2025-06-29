#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "function_principale.h"
#include "Factorisation_matricielle/factorisation_matricielle.h"


int main() {
    int num_transactions, m, n;
    Transaction *transactions = load_transactions("ratings.txt", &num_transactions, &m, &n);

    if (!transactions) {
        printf("Échec du chargement des transactions.\n");
        return 1;
    }

    printf("Nombre de transactions : %d\n", num_transactions);
    printf("Nombre d'utilisateurs : %d\n", m);
    printf("Nombre d'items : %d\n", n);

    // Afficher quelques transactions pour vérifier
    /*for (int i = 0; i < num_transactions && i < 5; i++) {
        printf("Transaction %d: user=%d, item=%d, category=%d, rating=%.2f, timestamp=%ld\n",
               i, transactions[i].user_id, transactions[i].item_id,
               transactions[i].category_id, transactions[i].rating, transactions[i].timestamp);
    }*/

    for (int i = 0; i < num_transactions && i < 5; i++) {
        printf("Transaction %d: user=%d, item=%d, category=%d, rating=%.2f, timestamp=%ld\n",
               i, transactions[i].user_id, transactions[i].item_id,
               transactions[i].category_id, transactions[i].rating, transactions[i].timestamp);
    }

    // Libérer la mémoire
    free(transactions);
    return 0;
}