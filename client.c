#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#define PORT 8080

typedef struct
{
    int user_id;           // Identifiant de l'utilisateur  
    int nb_item;           // Nombre d'items à recommander
    char algorithm[10];    // Algorithme utilisé (taille fixe)
} client_t;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    int choice;

    while (1) {
        printf("\n=== Menu Client ===\n");
        printf("1. Envoyer une demande de recommandation\n");
        printf("0. Quitter\n");
        printf("Choix : ");
        scanf("%d", &choice);
        getchar(); // Consomme le caractère de nouvelle ligne

        if (choice == 0) {
            printf("Au revoir !\n");
            break; // Sortie du programme
        } else if (choice != 1) {
            printf("Option invalide. Veuillez réessayer.\n");
            continue;
        }

        // Initialisation de la structure client
        client_t client;
        printf("Entrez l'ID de l'utilisateur : ");
        scanf("%d", &client.user_id);
        getchar(); // Consomme le caractère de nouvelle ligne

        printf("Entrez le nombre d'items à recommander : ");
        scanf("%d", &client.nb_item);
        getchar(); // Consomme le caractère de nouvelle ligne

        printf("Entrez l'algorithme (max 9 caractères, ex: KNN, FM, GRAPHES) : ");
        fgets(client.algorithm, sizeof(client.algorithm), stdin);
        client.algorithm[strcspn(client.algorithm, "\n")] = 0; // Supprime le '\n'
        if (strlen(client.algorithm) == 0) {
            strcpy(client.algorithm, "KNN"); // Défaut si entrée vide
        }

        // Vérification de la longueur de l'algorithme
        if (strlen(client.algorithm) >= sizeof(client.algorithm)) {
            printf("L'algorithme est trop long. Utilisation de 'KNN' par défaut.\n");
            strcpy(client.algorithm, "KNN");
        }

        // Création de la socket
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("\n Erreur de création de socket \n");
            return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        // Conversion de l'adresse IP
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
            printf("\nAdresse invalide ou non supportée \n");
            close(sock);
            return -1;
        }

        // Connexion au serveur
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            fprintf(stderr, "\nÉchec de la connexion : %s (errno: %d)\n", strerror(errno), errno);
            close(sock);
            return -1;
        }

        // Envoi des données
        send(sock, &client, sizeof(client_t), 0);
        printf("Les données ont été envoyées avec succès\n\n");

        // Réception de la réponse
        if (read(sock, buffer, 1024) < 0) {
            fprintf(stderr, "Erreur de lecture : %s (errno: %d)\n", strerror(errno), errno);
            close(sock);
            return -1;
        }
        printf("Réponse du serveur : %s\n", buffer);

        // Fermeture de la socket
        close(sock);
    }

    return 0;
}
