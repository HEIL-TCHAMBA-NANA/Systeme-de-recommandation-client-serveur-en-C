#include<stdio.h>
#include<stdlib.h>
#include"graphes.h"
#include<stdbool.h>
#include<string.h>

save **get_data(char *filename, save** data, char *critere){

    Transaction elt;
    int i = 0,nbre = 0,j;
    FILE *f1 = fopen("size.txt","r");
        if (f1 == NULL){
            perror("Erreur lors de l'ouverture du fichier de lignes");
            exit(EXIT_FAILURE);
        }
        fscanf(f1,"%d",&nbre);
    fclose(f1);
    FILE *f = fopen(filename,"r");
        if (f == NULL){
            perror("Erreur lors de l'ouverture du fichier");
            exit(EXIT_FAILURE);
        }
        data = malloc(nbre*sizeof(save*));
        for (i = 0; i < nbre; i++)
            data[i] = NULL;
        i = 0;
        bool occuped = false;
        int position = 0;
        while (fscanf(f,"%d %d %d %f %ld",&elt.user_id,&elt.item_id,&elt.category_id,&elt.rating,&elt.timestamp) != EOF)
        {   
            occuped = false;
            position = 0;
            for (j = 0; j < i; j++){                 
                if (strcmp(critere,"user_id") == 0 && data[j] != NULL &&  data[j]->element.user_id == elt.user_id){
                    occuped = true;
                    position = j;
                    //printf("\nJ: %d ",j);
                    //printf("-- I: %d\n",i);
                }
                if (strcmp(critere,"item_id") == 0 && data[j] != NULL &&  data[j]->element.item_id == elt.item_id){
                    occuped = true;
                    position = j;
                }
            }

            if (occuped == false){

                save *tmp = malloc(sizeof(save));
                tmp->element.user_id = elt.user_id;
                tmp->element.item_id = elt.item_id;
                tmp->element.category_id = elt.category_id;
                tmp->element.rating = elt.rating;
                tmp->element.timestamp = elt.timestamp;
                tmp->next = NULL;
                data[i] = tmp;
                ++i;
                //printf("Tmp=%d->NULL\n",tmp->element.user_id);
            }
            else{
                save *tmp = malloc(sizeof(save));
                tmp->element.user_id = elt.user_id;
                tmp->element.item_id = elt.item_id;
                tmp->element.category_id = elt.category_id;
                tmp->element.rating = elt.rating;
                tmp->element.timestamp = elt.timestamp;
                //printf("data[%d]=%d --- tmp=%d\n",position,data[position]->element.user_id,tmp->element.user_id);
                tmp->next = data[position];
                data[position] = tmp;
                //printf("data[%d]=%d->data[%d]=%d\n",position,data[position]->element.user_id,position,data[position]->next->element.user_id);
            }
        }     
    fclose(f);
    return data;
}


/* =========================================================================================== */

data get_userIds_and_itemIds(char *filename,data data_list){
    
    int nbre, user,item,category, i, j;
    float rating;
    long timestamp;

    FILE *f = fopen(filename,"r");
        if (f == NULL){
            perror("Erreur lors de l'ouverture du fichier.");
            exit(0);
        }
        FILE *f1 = fopen("size.txt","r");
            if (f1 == NULL){
                perror("Erreur lors de l'ouverture du fichier de lignes");
                exit(EXIT_FAILURE);
            }
            fscanf(f1,"%d",&nbre);
        fclose(f1);
        data_list.user_ids = malloc(nbre*sizeof(int));
        data_list.item_ids = malloc(nbre*sizeof(int));
        for ( i = 0; i < nbre; i++)
        {
            data_list.item_ids[i] = 0;
            data_list.user_ids[i] = 0;
        }
        i = 0;
        j = 0;
        while (fscanf(f,"%d %d %d %f %ld",&user,&item,&category,&rating,&timestamp) != EOF)
        {
            if (recherche_lineaire(data_list.item_ids,user,0,nbre) == false)
            {
                data_list.item_ids[i] = item;
                ++i;
            }
            if (recherche_lineaire(data_list.user_ids,user,0,nbre) == false)
            {
                data_list.user_ids[j] = user;
                ++j;
            }
        }
        data_list.nbreItems = i;
        data_list.nbreUsers = j;
    fclose(f);
        return data_list;
}

/* =========================================================================================== */


bool recherche_lineaire(int *array, int finded,int start, int size){
    int i;
    for (i = start; i < size; i++)
    {
        if (array[i] == finded)
        {
            return true;
        }
    }
    return false; 
}

/* =========================================================================================== */


int **get_matrice_adjascence(char *filename, int **matrice){
    data data_list;
    data_list =  get_userIds_and_itemIds(filename, data_list);
    int i,j=0;
    int size = data_list.nbreItems+data_list.nbreUsers;
    matrice = malloc(size*sizeof(int *));
    
    for (i = 0; i < size; i++){
        matrice[i] = malloc(size*sizeof(int));
    }
    for (i = 0; i < data_list.nbreUsers; i++){
        for (j = 0; j < data_list.nbreUsers; j++){
            matrice[i][j] = 0;
        }
        int *tab;
        for (j = 0;j < data_list.nbreItems; j++)
        {   
            tab = get_itemsIds_per_userId(data_list.user_ids[i],filename,tab);
            if (recherche_lineaire(tab,data_list.item_ids[j],1,tab[0]) == true)
            {   
                matrice[i][j+data_list.nbreUsers] = 1;
            }
            else{
                matrice[i][j+data_list.nbreUsers] = 0;
            }            
        }
        
    }
    printf("\n--END-USERS--\n");
    for (i = data_list.nbreUsers; i < size; i++){
        for (j = data_list.nbreUsers; j < size; j++){
            matrice[i][j] = 0;
        }
        int *tab;
        for (j = 0;j < data_list.nbreUsers; j++)
        {   
            //printf("\n--BEGIN--\n");
            tab = get_userIds_per_itemId(data_list.item_ids[i],filename,tab);
            //printf("\n--BEGIN-1--\n");
            if (recherche_lineaire(tab,data_list.item_ids[j],1,tab[0]) == true)
            {   
                //printf("\n--TRUE-1--\n");
                matrice[i][j+data_list.nbreUsers] = 1;  
                //printf("\n--END-1--\n");
            }
            else{
                //printf("\n--FALSE-2--\n");
                matrice[i][j+data_list.nbreUsers] = 0;
                //printf("\n--END-2--\n");
            }            
            //printf("\n--END--\n");
        }
    }
    return matrice;
}

/* =========================================================================================== */

void afficher_matrice(int lignes, int colonnes, int **mat) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            printf("%d\t", mat[i][j]);
        }
        printf("\n");
    }
}

/* =========================================================================================== */

int *get_itemsIds_per_userId(int user_id, char *filename, int *tab){
    save **file_data = NULL;
    file_data = get_data(filename,file_data,"user_id");
    int nbre,i,j,size;

    FILE *f1 = fopen("size.txt","r");
        if (f1 == NULL){
            perror("Erreur lors de l'ouverture du fichier de lignes");
            exit(EXIT_FAILURE);
        }
        fscanf(f1,"%d",&nbre);
    fclose(f1);
    for (i = 0; i < nbre; i++)
    {
        if (file_data[i] != NULL && file_data[i]->element.user_id == user_id)
        {
            save *tmp = file_data[i];
            size = 0;
            while (tmp != NULL)
            {
                size += 1;
                tmp = tmp->next;
            }
            tab = malloc((size+1)*sizeof(int)); 
            j = 1;
            tmp = file_data[i];
            while (tmp != NULL)
            {
                tab[j] = tmp->element.item_id;
                ++j;
                tmp = tmp->next;
            }
            tab[0] = size+1;
            return tab;
        } 
    }
    return tab;
}

/* =========================================================================================== */

int *get_userIds_per_itemId(int item_id, char *filename, int *tab){
    save **file_data = NULL;
    file_data = get_data(filename,file_data,"item_id");
    int nbre,i,j,size;

    FILE *f1 = fopen("size.txt","r");
        if (f1 == NULL){
            perror("Erreur lors de l'ouverture du fichier de lignes");
            exit(EXIT_FAILURE);
        }
        fscanf(f1,"%d",&nbre);
    fclose(f1);
    for (i = 0; i < nbre; i++)
    {
        if (file_data[i] != NULL && file_data[i]->element.item_id == item_id)
        {
            save *tmp = file_data[i];
            size = 0;
            while (tmp != NULL)
            {
                size += 1;
                tmp = tmp->next;
            }

            tab = malloc((size+1)*sizeof(int)); 
            j = 1;
            tmp = file_data[i];
            while (tmp != NULL)
            {
                tab[j] = tmp->element.user_id;
                ++j;
                tmp = tmp->next;
            }
            tab[0] = size+1;
            return tab;
        } 
    }
    return tab;
}

/* =========================================================================================== */


int main(void){
    //char *filename = "data.txt";
    //save **elt = NULL;
    //elt = get_data(filename,elt,"item_id");
    // int *tab = NULL;
    // tab = get_itemsIds_per_userId(97,"data.txt",tab);
    // printf("\nTaille: %d\n",tab[0]);
    // for (int i = 1; i < tab[0]; i++)
    // {
    //     printf("Tab[%d]=%d -- ",i,tab[i]);
    // }
    // save *tmp;
    // for (int i = 0; i < 50; i++)
    // {   

    //     tmp = elt[i];
    //     while (tmp != NULL)
    //     {
    //         fprintf(stdout,"User: %d - item: %d - cat: %d -  rate: %f - date: %ld\n\n",tmp->element.user_id,tmp->element.item_id,tmp->element.category_id,tmp->element.rating,tmp->element.timestamp);
    //         tmp = tmp->next;
    //     }
        
    // }
    int **matrice = NULL;
    data data_list;
    data_list =  get_userIds_and_itemIds("data.txt",data_list);
    int taille = data_list.nbreItems+data_list.nbreUsers;
    printf("\nMATRICE-D'AJASCENCE\n");
    matrice = get_matrice_adjascence("data.txt",matrice);
    printf("\nAFFICHAGE\n");
    afficher_matrice(taille,taille,matrice);

}

