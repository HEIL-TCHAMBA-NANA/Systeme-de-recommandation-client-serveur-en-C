#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "function_principale.h"
//#include "Factorisation_matricielle/factorisation_matricielle.h"
#include "knn/knn.h"


int main() {

    periodic_transaction("train.txt", "01/01/2023", "31/12/2023");
    clean_test_file("ratings.txt", "test.txt");

    return 0;
}