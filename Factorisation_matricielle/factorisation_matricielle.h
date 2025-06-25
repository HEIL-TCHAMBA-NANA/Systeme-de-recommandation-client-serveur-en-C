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
#endif