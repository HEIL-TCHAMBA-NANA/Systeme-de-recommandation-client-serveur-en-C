#ifndef __FUNCTIONS_H__
    #define __FUNCTIONS_H__
    #include "../structures/struct_transaction.h"
    #include"stdbool.h"

    typedef struct {
        int *user_ids;
        int *item_ids;
        int nbreUsers;
        int nbreItems;
    } Data;

    //Transaction *load_transactions_graphes(const char *filename, int *num_transactions);
    Data get_unique_users_and_items(Transaction *transactions, int num_transactions);
    float** create_weighted_adjacency_matrix(Transaction *transactions, int num_transactions, Data data);
    void normalize_matrix(float **matrix, int size);
    float* personalized_pagerank(float **matrix, float *personalization, int size, float alpha, int max_iter);
    float** create_prediction_matrix(Data data, float **adj_matrix, float alpha);
    void print_prediction_matrix(float **matrix, Data data);
    float ** get_matrice_predictions(const char *filename, int num_transactions, Transaction *transactions,float alpha);
    


#endif