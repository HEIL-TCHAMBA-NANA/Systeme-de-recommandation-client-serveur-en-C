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
            perror("Erreur lors de l'ouverture du fichier de données.");
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
            for (j = 0; j <= i; j++){                 
                if (strcmp(critere,"user_id") == 0 && data[j] != NULL &&  data[j]->element.user_id == elt.user_id){
                    occuped = true;
                    position = j;
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
            if (recherche_lineaire(data_list.item_ids,item,0,nbre) == false)
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
    // printf("nbreUsers=%d\n",data_list.nbreUsers);
    // printf("nbreItems=%d\n",data_list.nbreItems);
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
    for (i = data_list.nbreUsers; i < size; i++){
        for (j = data_list.nbreUsers; j < size; j++){
            matrice[i][j] = 0;
        }
        int *tab;
        for (j = 0;j < data_list.nbreUsers; j++)
        {   
            tab = get_userIds_per_itemId(data_list.item_ids[i-data_list.nbreUsers],filename,tab);
            if (recherche_lineaire(tab,data_list.user_ids[j],1,tab[0]) == true)
            {   
                matrice[i][j] = 1;  
            }
            else{
                matrice[i][j+data_list.nbreUsers] = 0;
            }            
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

float** produit_matrices(float **A, float **B, int lignesA, int colonnesA, int lignesB) {
    // A est de taille (lignesA x colonnesA)
    // B est de taille (colonnesA x colonnesB)
    // Le résultat sera de taille (lignesA x colonnesB)

    float **resultat = malloc(lignesA * sizeof(int *));
    for (int i = 0; i < lignesA; i++) {
        resultat[i] = malloc(lignesB * sizeof(int));
        for (int j = 0; j < lignesB; j++) {
            resultat[i][j] = 0;
            for (int k = 0; k < colonnesA; k++) {
                resultat[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return resultat;
}


/* =========================================================================================== */

float** produit_scalaire(int **A, int lignes, int colonnes, float alpha) {
    float **resultat = malloc(lignes * sizeof(float *));
    for (int i = 0; i < lignes; i++) {
        resultat[i] = malloc(colonnes * sizeof(int));
        for (int j = 0; j < colonnes; j++) {
            resultat[i][j] = alpha * A[i][j];
        }
    }
    return resultat;
}

 float** produit_scalaire_float(float **A, int lignes, int colonnes, float alpha){
    float **resultat = malloc(lignes * sizeof(float *));
    for (int i = 0; i < lignes; i++) {
        resultat[i] = malloc(colonnes * sizeof(int));
        for (int j = 0; j < colonnes; j++) {
            resultat[i][j] = alpha * A[i][j];
        }
    }
    return resultat;
 }


/* =========================================================================================== */

float** somme_matrices(float **A, float **B, int lignes, int colonnes) {
    float **resultat = malloc(lignes * sizeof(int *));
    if (resultat == NULL) {
        perror("Erreur malloc résultat");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < lignes; i++) {
        resultat[i] = malloc(colonnes * sizeof(int));
        if (resultat[i] == NULL) {
            perror("Erreur malloc ligne résultat");
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < colonnes; j++) {
            resultat[i][j] = A[i][j] + B[i][j];
        }
    }

    return resultat;
}


/* =========================================================================================== */

float **pageRank(float **pr,int **matrice_adjacence,float alpha,float **d, char *filename){
    data file_data;
    int size,i,j;
    float **pr_new;
    printf("\nPAGE-RANK\n");
    matrice_adjacence = get_matrice_adjascence(filename,matrice_adjacence);
    printf("\nPAGE-1\n");
    file_data = get_userIds_and_itemIds(filename,file_data);
    printf("\nPAGE-2\n");
    size = file_data.nbreItems+file_data.nbreUsers;
    pr = malloc(size*sizeof(int*));
    
    for (i = 0; i < size; i++){
        pr[i] = malloc(sizeof(int));
        pr[i][0] = 1/size;
    }

    pr_new = malloc(size*sizeof(float*));
    for (i = 0; i < size; i++)
    {
        pr_new[i] = malloc(size*sizeof(float));
        for (j = 0; j < size; j++)
        {
            pr_new[i][j] = 0;
        }
    }
    

    printf("\nPAGE-3\n");
    pr_new = produit_scalaire(matrice_adjacence,size,size,alpha);
    printf("\nPAGE-3--1\n");
    pr = produit_matrices(pr_new,pr,1,size,size);
    for (i = 0; i < size; i++)
    {
        printf("i = %d",i);
        printf("pr[%d] = %f\n",i,pr[0][i]);
    }
    printf("\nPAGE-4\n");
    d = produit_scalaire_float(d,1,size,1-alpha);
    printf("\nPAGE-5\n");
    pr_new = somme_matrices(pr,d,1,size);
    for (i = 0; i < size; i++)
    {
        printf("i = %d",i);
        printf("pr[%d] = %f\n",i,pr[0][i]);
    }
    printf("\nPAGE-6\n");
    /*for (i = 0; i < max_iter; i++)
    {   
        printf("\nBOUCLE-1\n");
        pr = produit_scalaire_float(pr,size,1,alpha);
        printf("\nBOUCLE-1--1\n");
        pr = produit_matrices(pr_new,pr,size,size,size);
        printf("\nBOUCLE-2\n");
        d = produit_scalaire_float(d,size,1,1-alpha);
        printf("\nBOUCLE-3\n");
        pr = somme_matrices(pr,d,1,size);
        printf("\nBOUCLE-4\n");
    }*/
    printf("\nPAGE-7\n");

    return pr;
}


/* =========================================================================================== */


int main(){
    char *filename = "data.txt";
    data file_data;
    int size,user_id,i;
    float **d,**pr=NULL;
    int **matrice_adjacence = NULL;
    int alpha = 0.85;//max_iter = 50;

    file_data = get_userIds_and_itemIds(filename,file_data);
    size = file_data.nbreItems+file_data.nbreUsers;
    printf("Veuillez entrer votre userID: ");
    user_id = scanf("%d",&user_id);
    printf("User-id-1 = %d\n",user_id);
    user_id = 322;
    d = malloc(size*sizeof(float*));
    for (i = 0; i < size; i++)
    {
        d[i] = malloc(sizeof(float));
        d[i][0] = 0;
    }
    
    //pr[0] = malloc((size)*sizeof(float));

    printf("\n--1--\n");

    if (recherche_lineaire(file_data.user_ids,user_id,0,file_data.nbreUsers) == false){
        printf("User-id = %d\n",user_id);
        return file_data.item_ids[rand()%file_data.nbreItems];
    }

    printf("\n--2--\n");

    for (i = 0; i < file_data.nbreUsers; i++)
        if (file_data.user_ids[i] == user_id)
            d[0][i] = 1;
    
    printf("\n--3--\n");

    pr = pageRank(pr,matrice_adjacence,alpha,d,filename);

    printf("\n--4--\n");

    for (i = 0; i < size; i++)
    {
        printf("i = %d",i);
        printf("pr[%d] = %f\n",i,pr[0][i]);
    }

    printf("\n--5--\n");
    
    
    return 0;
}