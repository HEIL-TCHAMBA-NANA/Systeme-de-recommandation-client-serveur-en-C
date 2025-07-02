#ifndef _INCLUDE_FUNCTION_PRINCIPALE_H
#define _INCLUDE_FUNCTION_PRINCIPALE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "structures/struct_transaction.h"


//Ici on mettra les fonctions qui ne sont pas liees a une bibliotheque particulière comme par exemple les fonctions de manipulation de fichiers.
Transaction *load_transactions(const char *filename, int *num_transactions, int *m, int *n);

long date_to_timestamp(const char *date_str);
void periodic_transaction(const char *filename, const char *debut_str, const char *fin_str);
void delete_transaction(const char *filename, int min_U, int min_I);
void clean_test_file(const char *training, const char *test);

void get_top_n_recommendations(float **full_matrix, int user_id, int m, int n, int N, int *top_items);


#endif