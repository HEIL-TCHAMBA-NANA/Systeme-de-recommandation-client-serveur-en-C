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


    int **get_matrice_adjascence(char *filename);
    data get_userIds_and_itemIds(char *filename);//Récuperer la liste des items et user_ids présents dans le jeu de données
    bool recherche_lineaire(int* array,int array_size,int finded);// Recherche d'une valeur dans un tableau d'entiers
    save **get_data(char *filename,save**); // Récuperer la liste des données du fichier et retourner dans un tableau de liste-chainées 
    

#endif