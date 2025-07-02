#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "function_principale.h"
#include "Factorisation_matricielle/factorisation_matricielle.h"
#include "knn/knn.h"

// Même structure que côté client
typedef struct
{
    int user_id;           // Identifiant de l'utilisateur
    int nb_item;           // Nombre d'items à recommander  
    char algorithm[10];    // Algorithme utilisé (taille fixe)
} client_t;

int main() {
    // Charger les données
    int nb_transactions_entrainement, nb_utilisateurs, nb_items;
    Transaction *donnees_entrainement = load_transactions("train.txt", &nb_transactions_entrainement, &nb_utilisateurs, &nb_items);
    if (!donnees_entrainement) {
        fprintf(stderr, "Échec du chargement des données d'entraînement\n");
        return 1;
    }

    // Préparer la matrice pour KNN
    float **matrice_entrainement_knn = Generate_matrix_knn(donnees_entrainement, nb_transactions_entrainement, nb_utilisateurs, nb_items);
    if (!matrice_entrainement_knn) {
        fprintf(stderr, "Échec de la génération de la matrice des transactions\n");
        free(donnees_entrainement);
        return 1;
    }

    // Entraîner le modèle FM
    MF_Params parametres = {0.01, 0.1, 100};
    int k = 10;
    float **matrice_complete_fm = MF(donnees_entrainement, nb_transactions_entrainement, &parametres, nb_utilisateurs, nb_items, k);
    if (!matrice_complete_fm) {
        fprintf(stderr, "Échec de la factorisation matricielle\n");
        liberer_matrice(matrice_entrainement_knn, nb_utilisateurs);
        free(donnees_entrainement);
        return 1;
    }

    // Configurer le socket serveur
    int socket_serveur, nouvelle_connexion;
    struct sockaddr_in adresse;
    int taille_adresse = sizeof(adresse);
    if ((socket_serveur = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Échec de la création du socket");
        liberer_matrice(matrice_entrainement_knn, nb_utilisateurs);
        free_full_matrix(matrice_complete_fm, nb_utilisateurs);
        free(donnees_entrainement);
        return 1;
    }

    adresse.sin_family = AF_INET;
    adresse.sin_addr.s_addr = INADDR_ANY;
    adresse.sin_port = htons(8080);
    if (bind(socket_serveur, (struct sockaddr *)&adresse, sizeof(adresse)) < 0) {
        perror("Échec du bind");
        close(socket_serveur);
        liberer_matrice(matrice_entrainement_knn, nb_utilisateurs);
        free_full_matrix(matrice_complete_fm, nb_utilisateurs);
        free(donnees_entrainement);
        return 1;
    }

    if (listen(socket_serveur, 3) < 0) {
        perror("Échec du listen");
        close(socket_serveur);
        liberer_matrice(matrice_entrainement_knn, nb_utilisateurs);
        free_full_matrix(matrice_complete_fm, nb_utilisateurs);
        free(donnees_entrainement);
        return 1;
    }

    printf("Serveur en écoute sur le port 8080...\n");

    // Boucle pour accepter les connexions
    while (1) {
        if ((nouvelle_connexion = accept(socket_serveur, (struct sockaddr *)&adresse, (socklen_t*)&taille_adresse)) < 0) {
            perror("Échec de l'accept");
            continue;
        }

        // Lire la structure client_t
        client_t client_recu;
        int valread = read(nouvelle_connexion, &client_recu, sizeof(client_t));
        if (valread <= 0) {
            perror("Échec de la lecture de la requête");
            close(nouvelle_connexion);
            continue;
        }

        printf("Requête reçue - User ID: %d, Nb items: %d, Algorithm: %s\n", 
               client_recu.user_id, client_recu.nb_item, client_recu.algorithm);

        int id_utilisateur = client_recu.user_id;
        int N = client_recu.nb_item;
        char reponse[1024] = {0};

        if (id_utilisateur >= 0 && id_utilisateur < nb_utilisateurs && N > 0 && N <= nb_items) {
            int *top_items = malloc(N * sizeof(int));
            if (!top_items) {
                snprintf(reponse, sizeof(reponse), "Erreur d'allocation mémoire\n");
            } else if (strcmp(client_recu.algorithm, "FM") == 0) {
                get_top_n_recommendations(matrice_complete_fm, id_utilisateur, nb_utilisateurs, nb_items, N, top_items);
                char temp[256];
                snprintf(reponse, sizeof(reponse), "Top %d items pour l'utilisateur %d (FM) : ", N, id_utilisateur);
                for (int i = 0; i < N && top_items[i] != -1; i++) {
                    snprintf(temp, sizeof(temp), "%d (%.2f), ", top_items[i], matrice_complete_fm[id_utilisateur][top_items[i]]);
                    strncat(reponse, temp, sizeof(reponse) - strlen(reponse) - 1);
                }
                strncat(reponse, "\n", sizeof(reponse) - strlen(reponse) - 1);
                free(top_items);
            } else if (strcmp(client_recu.algorithm, "KNN") == 0) {
                get_top_n_recommendations_knn(matrice_entrainement_knn, id_utilisateur, nb_utilisateurs, nb_items, N, top_items);
                char temp[256];
                snprintf(reponse, sizeof(reponse), "Top %d items pour l'utilisateur %d (KNN) : ", N, id_utilisateur);
                for (int i = 0; i < N && top_items[i] != -1; i++) {
                    float note = Predict(id_utilisateur, top_items[i], matrice_entrainement_knn, nb_utilisateurs, nb_items);
                    snprintf(temp, sizeof(temp), "%d (%.2f), ", top_items[i], note);
                    strncat(reponse, temp, sizeof(reponse) - strlen(reponse) - 1);
                }
                strncat(reponse, "\n", sizeof(reponse) - strlen(reponse) - 1);
                free(top_items);
            } else {
                snprintf(reponse, sizeof(reponse), "Algorithme invalide: %s (supportés: FM, KNN)\n", client_recu.algorithm);
            }
        } else {
            snprintf(reponse, sizeof(reponse), "Indices ou N invalides (User ID: %d, N: %d, Max users: %d, Max items: %d)\n", 
                     id_utilisateur, N, nb_utilisateurs, nb_items);
        }

        // Envoyer la réponse
        if (send(nouvelle_connexion, reponse, strlen(reponse), 0) < 0) {
            perror("Échec de l'envoi de la réponse");
        }
        close(nouvelle_connexion);
    }

    // Nettoyage
    liberer_matrice(matrice_entrainement_knn, nb_utilisateurs);
    free_full_matrix(matrice_complete_fm, nb_utilisateurs);
    free(donnees_entrainement);
    close(socket_serveur);
    return 0;
}