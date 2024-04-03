/* fichiers de la bibliothèque standard */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
/* bibliothèque standard unix */
#include <unistd.h> /* close, read, write */
#include <sys/types.h>
#include <sys/socket.h>
/* spécifique à internet */
#include <arpa/inet.h> /* inet_pton */
/* spécifique aux comptines */
#include "comptine_utils.h"

#define PORT_WCP 4321

void usage(char *nom_prog)
{
	fprintf(stderr, "Usage: %s addr_ipv4\n"
			"client pour WCP (Wikicomptine Protocol)\n"
			"Exemple: %s 208.97.177.124\n", nom_prog, nom_prog);
}
int count_newlines(const char *start, const char *end);


/** Retourne (en cas de succès) le descripteur de fichier d'une socket
 *  TCP/IPv4 connectée au processus écoutant sur port sur la machine d'adresse
 *  addr_ipv4 */
int creer_connecter_sock(char *addr_ipv4, uint16_t port);

/** Lit la liste numérotée des comptines dans le descripteur fd et les affiche
 *  sur le terminal.
 *  retourne : le nombre de comptines disponibles */
uint16_t recevoir_liste_comptines(int fd);

/** Demande à l'utilisateur un nombre entre 0 (compris) et nc (non compris)
 *  et retourne la valeur saisie. */
uint16_t saisir_num_comptine(uint16_t nb_comptines);

/** Écrit l'entier ic dans le fichier de descripteur fd en network byte order */
void envoyer_num_comptine(int fd, uint16_t nc);

/** Affiche la comptine arrivant dans fd sur le terminal */
void afficher_comptine(int fd);


int main(int argc, char *argv[])
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    char *addr_ipv4 = argv[1];

    int sockfd = creer_connecter_sock(addr_ipv4, PORT_WCP);
    if (sockfd < 0) {
        printf("Erreur lors de la connexion au serveur\n");
        return 1;
    }
    uint16_t nb_comptines = recevoir_liste_comptines(sockfd);
    if (nb_comptines == 0) {
    	printf("Aucune comptine disponible\n");
    	close(sockfd);
    	return 0;
    }

    
    uint16_t num_comptine = saisir_num_comptine(nb_comptines);
    envoyer_num_comptine(sockfd, num_comptine);

    // Recevoir et afficher la comptine demandée
    printf("Comptine sélectionnée :\n");
    afficher_comptine(sockfd);
    printf("\n");

    close(sockfd);

    return 0;
}




int creer_connecter_sock(char *addr_ipv4, uint16_t port)
{
	    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        return -1;
    }

    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, addr_ipv4, &client_addr.sin_addr) <= 0) {
        perror("adresse non valide");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0) {
        perror(" erreur connect");
        close(sockfd);
        return -1;
    }

    return sockfd;
}


uint16_t recevoir_liste_comptines(int fd)
{
    int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    uint16_t nb_comptines = 0;
    int newline_count = 0;
    int fin_liste_trouve = 0;

    while (!fin_liste_trouve) {
        int bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read <= 0) {
            break;
        }

        // Afficher les données reçues
        if (write(STDOUT_FILENO, buffer, bytes_read) < 0) {
            perror("Erreur d'écriture");
            return 0;
        }

        newline_count += count_newlines(buffer, buffer + bytes_read);

        // Vérifier si la fin de la liste des comptines est présente
        if (newline_count >= 6) {
            fin_liste_trouve = 1;
        }
    }

    nb_comptines = newline_count - 1;  // Soustraire 1 pour le saut de ligne final

    return nb_comptines;
}




int count_newlines(const char *start, const char *end) {
    int count = 0;
    while (start < end) {
        if (*start == '\n') {
            count++;
        }
        start++;
    }
    return count;
}







    
uint16_t saisir_num_comptine(uint16_t nb_comptines)
{
    uint16_t num_comptine;

    while (1) {
        printf("Quelle comptine voulez-vous ? (Entrer un entier entre 0 et %" PRIu16 ") : ", nb_comptines );
        int ret = scanf("%hu", &num_comptine);


        if (ret == 1 && num_comptine >= 0 && num_comptine <= nb_comptines) {
            break;
        } else {
            printf("Nombre invalide. Veuillez réessayer.\n");
            // Vider le buffer d'entrée en cas de saisie invalide
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    }

    return num_comptine;
}



void envoyer_num_comptine(int fd, uint16_t nc)
{
    uint16_t nc_network = htons(nc);
    write(fd, &nc_network, sizeof(nc_network));
}




void afficher_comptine(int fd) {
    char buffer[1024];
    ssize_t bytes_read;
    ssize_t total_bytes_read = 0;

    while ((bytes_read = read(fd, buffer + total_bytes_read, sizeof(buffer) - total_bytes_read - 1)) > 0) {
        total_bytes_read += bytes_read;
        buffer[total_bytes_read] = '\0'; // Ajouter un caractère nul à la fin pour terminer la chaîne

        printf("%s", buffer);

        // Réinitialiser le nombre total de bytes lus pour la prochaine itération
        total_bytes_read = 0;
    }

    if (bytes_read < 0) {
        perror("Erreur lors de la lecture de la comptine");
    }
}





