#ifndef __GRAPHES_H__
    #define __GRAPHES_H__
    #include"../function_principale.h"
    #include"stdbool.h"

    typedef struct save{
        Transaction element;
        struct save *next;
    } save;

    typedef struct data{
        int *user_ids;
        int *item_ids;
        int nbreUsers;
        int nbreItems;
    }data;


    int **get_matrice_adjascence(char *filename, int**);
    data get_userIds_and_itemIds(char *filename,data data_list);//Récuperer la liste des items et user_ids présents dans le jeu de données
    bool recherche_lineaire(int* array,int finded, int start, int end);// Recherche d'une valeur dans un tableau d'entiers
    save **get_data(char *filename,save**,char *critere); /*Récuperer la liste des données du fichier et retourner dans un tableau 
                                de liste-chainées de sorte que l'on visulaise toutes les transactions ayant été éffectuées et les insérer dans la liste selon des critères prticuliers
                                */
    int *get_itemsIds_per_userId(int user_id,char *filename, int *tab); //Récuperer le tableau des item_id associés à un user_id avec la taille de la liste dans la première case 
    int *get_userIds_per_itemId(int item_id, char *filename, int *tab); //Récuperer le tableau des user_id associés à un item_id avec la taille de la liste dans la première case 
    void afficher_matrice(int lignes, int colonnes, int **mat) ;

    float** produit_matrices(float **A, float **B, int lignesA, int colonnesA, int lignesB);
    float** produit_scalaire(int **A, int lignes, int colonnes, float alpha);
    float** produit_scalaire_float(float **A, int lignes, int colonnes, float alpha);
    float** somme_matrices(float **A, float **B, int lignes, int colonnes);

    float **pageRank(float **pr,int **matrice_adjacence,float alpha,float **d,char *filename);
    float** get_matrice_prediction(char *filename, float alpha);
#endif