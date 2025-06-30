#include<stdio.h>
#include<stdlib.h>
#include"graphes.h"
#include<stdbool.h>

save **get_data(char *filename, save** data){

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
                if (data[j] != NULL &&  data[j]->element.user_id == elt.user_id){
                    occuped = true;
                    position = j;
                }
            }

            if (occuped == false){

                i++;
                save *tmp = malloc(sizeof(save));
                tmp->element.user_id = elt.user_id;
                tmp->element.item_id = elt.item_id;
                tmp->element.category_id = elt.category_id;
                tmp->element.rating = elt.rating;
                tmp->element.timestamp = elt.timestamp;
                tmp->next = NULL;
                data[i] = tmp;
            }
            else{
                save *tmp = malloc(sizeof(save));
                tmp->element.user_id = elt.user_id;
                tmp->element.item_id = elt.item_id;
                tmp->element.category_id = elt.category_id;
                tmp->element.rating = elt.rating;
                tmp->element.timestamp = elt.timestamp;
                tmp->next = data[position];
                data[position] = tmp;
            }
        }     
    feof(f);
    return data;
}

/* =========================================================================================== */

data get_userIds_and_itemIds(char *filename){
    data data_list;
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
        if (recherche_lineaire(data_list.item_ids,nbre,user) == false)
        {
            i++;
            data_list.item_ids[i] = item;
        }
        if (recherche_lineaire(data_list.user_ids,nbre,user) == false)
        {
            j++;
            data_list.user_ids[j] = user;
        }
    }
    data_list.nbreItems = i;
    data_list.nbreUsers = j;
    return data_list;
}

/* =========================================================================================== */


bool recherche_lineaire(int *array, int size, int finded){
    int i;
    for (i = 0; i < size; i++)
    {
        if (array[i] == finded)
        {
            return true;
        }
    }
    return false; 
}

/* =========================================================================================== */



/* =========================================================================================== */


int main(void){
    char *filename = "data.txt";
    // save **elt = NULL;
    // elt = get_data(filename,elt);
    
    // for (int i = 0; i < 50; i++)
    // {   
    //     if (elt[i] != NULL && elt[i]->next == NULL){   
    //         fprintf(stdout,"User: %d - item: %d - cat: %d -  rate: %f - date: %ld\n\n",elt[i]->element.user_id,elt[i]->element.item_id,elt[i]->element.category_id,elt[i]->element.rating,elt[i]->element.timestamp);
    //     }
    //     else
    //         if(elt[i] != NULL){
    //             save *tmp = elt[i];
    //             while (tmp != NULL)
    //             {
    //                 fprintf(stdout,"User: %d - item: %d - cat: %d -  rate: %f - date: %ld\n\n",elt[i]->element.user_id,elt[i]->element.item_id,elt[i]->element.category_id,elt[i]->element.rating,elt[i]->element.timestamp);
    //                 tmp = tmp->next;
    //             }
    //         }
    // }

}

