#ifndef _FACTORISATION_MATRICIELLE_H
#define _FACTORISATION_MATRICIELLE_H


// Structure pour les matrices U, V et biais
typedef struct {
    float **U;       
    float **V;     
    float *O;        
    float *P;        
    int m;           
    int n;           
    int k;           
} MF_Model;

// Structure pour les paramètres de l'algorithme
typedef struct {
    float alpha;     
    float lambda;    
    int num_epochs;
} MF_Params;

void initialize_mf_model(MF_Model *model, int m, int n, int k);
void train_mf_model(MF_Model *model, Transaction *transactions, int num_transactions, MF_Params *params);
float predict_mf(MF_Model *model, int user_id, int item_id);
float **generate_full_matrix(MF_Model *model);
void predict_all_mf(float **full_matrix, Transaction *test_data, int num_test, float *predictions);
void evaluate_mf(Transaction *test_data, int num_test, float *predictions, float *rmse, float *mae);
#endif