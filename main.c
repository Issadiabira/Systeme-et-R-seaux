
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFSZ 256


int read_until_nl(int fd, char *buf);

int main(){
	int fd = open("lol.txt", O_RDONLY);
	int n;
	
	char buffer[BUFSZ];
	n=read_until_nl(fd,buffer);
	printf("le nombre d'octet lu est:%d\n", n);
	printf("les octets lu   du fichier : %s\n", buffer);

}

int read_until_nl(int fd, char *buf) {
    int nb_octets_lus = 0;
    int nb_octets_total_lus = 0;
    char c = '\0';
    int taille_buf=256;

    while (nb_octets_total_lus < taille_buf - 1) { // -1 pour laisser de la place pour '\0'
        ssize_t res = read(fd, &c, 1);
        if (res == -1) {
            perror("Erreur de lecture du fichier");
            exit(EXIT_FAILURE);
        }
        if (res == 0 || c == '\n') { // fin du fichier ou caractère de nouvelle ligne trouvé
            buf[nb_octets_lus] = '\0';
            return nb_octets_lus;
        }
        buf[nb_octets_lus] = c;
        nb_octets_lus++;
        nb_octets_total_lus++;
    }

    buf[nb_octets_lus] = '\0';
    return nb_octets_lus;
}

	
	






