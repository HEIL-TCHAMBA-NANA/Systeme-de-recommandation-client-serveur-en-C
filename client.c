#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#define PORT 8080

typedef struct
{
    int user_id;  // Identifiant de l'utilisateur
    int nb_items; // Nombre d'items recommandés
    char *algorithm; // Algorithme utilisé pour la recommandation
} client_t;


int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    client_t client;
    client.user_id = 1;  // Exemple d'ID utilisateur
    client.nb_items = 2;  // Exemple d'ID item
    client.algorithm = "KNN"; // Exemple d'algorithme

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "10.2", &serv_addr.sin_addr) <= 0 ) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "\nConnection Failed: %s (errno: %d)\n", strerror(errno), errno);
        return -1;
    }
    
    send(sock, &client, sizeof(client_t), 0);
    printf("Données envoyées: user_id=%d, nb_items=%d, algorithm=%s\n", client.user_id, client.nb_items, client.algorithm);
    
    //send(sock, &client.user_id, sizeof(client.user_id), 0);
    //send(sock, &client.nb_items, sizeof(client.nb_items), 0);
    //send(sock, client.algorithm, strlen(client.algorithm), 0);

    //printf("Les données ont été envoyées avec succès \n\n");
    
    int valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);

    close(sock);
    return 0;
}