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

    client_t client;
    client.user_id = 1;     // Exemple d'ID utilisateur
    client.nb_item = 4;     // Exemple de nombre d'items
    strcpy(client.algorithm, "KNN"); // Copier l'algorithme

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "192.168.171.81", &serv_addr.sin_addr) <= 0 ) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "\nConnection Failed: %s (errno: %d)\n", strerror(errno), errno);
        return -1;
    }

    send(sock, &client, sizeof(client_t), 0);
    printf("Les données ont été envoyées avec succès \n\n");
    
    int valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
    

    close(sock);
    return 0;
}