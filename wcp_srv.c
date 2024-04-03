/*ISSA DIABIRA  je d'éclare qu'il s'agit de mon propre travail*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <dirent.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "comptine_utils.h"

#define PORT_WCP 4321

struct client_info {
    int sockfd;
    struct sockaddr_in client_addr;
    struct catalogue *cat;
    char *dirname;
};

void usage(char *nom_prog)
{
    fprintf(stderr, "Usage: %s repertoire_comptines\n"
                    "serveur pour WCP (Wikicomptine Protocol)\n"
                    "Exemple: %s comptines\n", nom_prog, nom_prog);
}

int creer_configurer_sock_ecoute(uint16_t port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erreur de création socket");
        return -1;
    }

    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("Erreur setsockopt");
        close(sockfd);
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Erreur bind");
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, SOMAXCONN) < 0) {
        perror("Erreur listen");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

void envoyer_liste(int fd, struct catalogue *c)
{
    for (int i = 0; i < c->nb; i++) {
        int ret = dprintf(fd, "%6d %s", i, c->tab[i]->titre);
        if (ret < 0) {
            perror("Erreur d'écriture");
            return;
        }
    }
}

uint16_t recevoir_num_comptine(int fd)
{
    uint16_t num_comptine;
    read(fd, &num_comptine, sizeof(num_comptine));
    return ntohs(num_comptine);
}

void envoyer_comptine(int fd, const char *dirname, struct catalogue *c, uint16_t ic)
{
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s.cpt", dirname, c->tab[ic]->nom_fichier);

    int filefd = open(filepath, O_RDONLY);
    if (filefd < 0) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    char buffer[1024];
    ssize_t bytes_read;

    while ((bytes_read = read(filefd, buffer, sizeof(buffer))) > 0) {
        write(fd, buffer, bytes_read);
    }

    close(filefd);
}

void *traiter_client(void *arg)
{
    struct client_info *client = (struct client_info *) arg;
    char client_addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client->client_addr.sin_addr, client_addr_str, INET_ADDRSTRLEN);
    printf("Connexion acceptée depuis %s:%d\n", client_addr_str, ntohs(client->client_addr.sin_port));

    uint16_t nb_comptines = client->cat->nb;
    envoyer_liste(client->sockfd, client->cat);

    uint16_t num_comptine = recevoir_num_comptine(client->sockfd);
    if (num_comptine >= nb_comptines) {
        printf("Numéro de comptine invalide\n");
        close(client->sockfd);
        free(client);
        return NULL;
    }

    envoyer_comptine(client->sockfd, client->dirname, client->cat, num_comptine);

    close(client->sockfd);
    free(client);

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    struct catalogue *cat = creer_catalogue(argv[1]);
    if (cat == NULL) {
        fprintf(stderr, "Erreur : impossible de créer le catalogue\n");
        return 1;
    }

    int sockfd = creer_configurer_sock_ecoute(PORT_WCP);
    if (sockfd < 0) {
        printf("Erreur lors de la création du socket d'écoute\n");
        return 1;
    }

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_sockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
        if (client_sockfd < 0) {
            perror("Erreur lors de l'acceptation de la connexion");
            close(sockfd);
            return 1;
        }

        struct client_info *client = malloc(sizeof(struct client_info));
        client->sockfd = client_sockfd;
        client->client_addr = client_addr;
        client->cat = cat;
        client->dirname = argv[1];

        pthread_t thread;
        if (pthread_create(&thread, NULL, traiter_client, (void *)client) != 0) {
            perror("Erreur lors de la création du thread");
            close(client_sockfd);
            free(client);
        }
    }

    close(sockfd);

    return 0;
}

