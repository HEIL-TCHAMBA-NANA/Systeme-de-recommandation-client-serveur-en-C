#ifndef _INCLUDE_FUNCTION_PRINCIPALE_H
#define _INCLUDE_FUNCTION_PRINCIPALE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "structures/struct_transaction.h"


//Ici on mettra les fonctions qui ne sont pas liees a une bibliotheque particulière comme par exemple les fonctions de manipulation de fichiers.
Transaction *load_transactions(const char *filename, int *num_transactions, int *m, int *n);
void get_top_n_recommendations(float **full_matrix, int user_id, int m, int n, int N, int *top_items);

#endif