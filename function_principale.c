#include<stdio.h>
#include<stdlib.h>
#include"function_principale.h"
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
        while (fscanf(f,"%d %d %d %f %ld",&elt.user_id,&elt.item_id,&elt.category_id,&elt.rating,&elt.timestamp) != EOF)
        {   
            bool occuped = false;
            int position = 0;
            
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

int main(void){

    char *filename = "data.txt";
    //printf("\nBegin\n");
    save **elt = NULL;
    elt = get_data(filename,elt);
    //printf("\nBegin_1\n");
    for (int i = 0; i < 50; i++)
    {   
        //printf("\n---1---\n");
        if (elt[i] != NULL && elt[i]->next == NULL)
        {   
            //printf("\nEntree---1_2---\n");
            fprintf(stdout,"User: %d - item: %d - cat: %d -  rate: %f - date: %ld\n\n",elt[i]->element.user_id,elt[i]->element.item_id,elt[i]->element.category_id,elt[i]->element.rating,elt[i]->element.timestamp);
            //printf("\nSortie---1_2---\n");
        }
        else
            if(elt[i] != NULL){
                //printf("\nEntree---1_3---\n");
                save *tmp = elt[i];
                while (tmp != NULL)
                {
                    fprintf(stdout,"User: %d - item: %d - cat: %d -  rate: %f - date: %ld\n\n",elt[i]->element.user_id,elt[i]->element.item_id,elt[i]->element.category_id,elt[i]->element.rating,elt[i]->element.timestamp);
                    tmp = tmp->next;
                }
                //printf("\nSortie---1_3---\n");
            }
    }        
    //printf("\nend\n");

}