/*ISSA DIABIRA je d'éclare qu'il s'agit de mon propre travail*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "comptine_utils.h"

int read_until_nl(int fd, char *buf)
{
	
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


int est_nom_fichier_comptine(char *nom_fich)
{
	 char *ext = strrchr(nom_fich, '.');  // Trouver la dernière occurrence du caractère '.' dans le nom de fichier
    if (ext != NULL && strcmp(ext, ".cpt") == 0) {  // Vérifier si l'extension est ".cpt"
        return 1;  // Extension valide
    }
    return 0;  // Extension invalide
}
	

	



struct comptine *init_cpt_depuis_fichier(const char *dir_name,const char *base_name){

	
    char repertoire[1024];
    int taille;
    sprintf(repertoire, "%s/%s", dir_name, base_name);
    
    FILE* fichier = fopen(repertoire, "r");
    if (fichier == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", repertoire);
        return NULL;
    }
    
    struct comptine* comptine =  malloc(sizeof(struct comptine));
    if (comptine == NULL) {
        printf("Erreur : Impossible d'allouer la structure Comptine\n");
        fclose(fichier);
        return NULL;
    }
    
    char ligne[1024];
    if (fgets(ligne, 1024, fichier) == NULL) {
        printf("Erreur : Impossible de lire la première ligne du fichier\n");
        fclose(fichier);
        return NULL;
    }
    comptine->titre = (char*) malloc(strlen(ligne) + 1);
    if (comptine->titre == NULL) {
        printf("Erreur : Impossible d'allouer la chaine de titre\n");
        fclose(fichier);
        return NULL;
    }
    strcpy(comptine->titre, ligne);
    
    comptine->nom_fichier = (char*) malloc(strlen(base_name) + 1);
    if (comptine->nom_fichier == NULL) {
        printf("Erreur : Impossible d'allouer la chaine de nom de fichier\n");
        fclose(fichier);
        return NULL;
    }
    strcpy(comptine->nom_fichier, base_name);
    
    taille = 1;
    while (fgets(ligne, 1024, fichier) != NULL) {
        taille++;
    }
    
    fclose(fichier);
    return comptine;
}


void liberer_comptine(struct comptine *cpt)
{
	if (cpt->nom_fichier != NULL){
		free(cpt->nom_fichier);
	}
	if (cpt->titre != NULL){
		free(cpt->titre);
	}
	free(cpt);
}

struct catalogue *creer_catalogue(const char *dir_name)
{
	
	// Ouvrir le répertoire
	DIR* dir = opendir(dir_name);
	if (dir == NULL) {
		fprintf(stderr, "Erreur : impossible d'ouvrir le répertoire %s\n", dir_name);
        	return NULL;
	}
    
    // Allouer de la mémoire pour la structure de catalogue
	struct catalogue* cat = (struct catalogue*) malloc(sizeof(struct catalogue));
	if (cat == NULL) {
		fprintf(stderr, "Erreur : impossible d'allouer de la mémoire pour le catalogue\n");
		closedir(dir);
		return NULL;
	}

	// Allouer de la mémoire pour le tableau de pointeurs vers des comptines
	int nombre_fichier = 120;
	cat->tab =  malloc(nombre_fichier * sizeof(struct comptine*));
	if (cat->tab == NULL) {
		fprintf(stderr, "Erreur : impossible d'allouer de la mémoire pour les pointeurs vers les comptines\n");
		closedir(dir);
		return NULL;
	}

	// Initialiser le nombre de fichiers comptine à 0
	cat->nb = 0;

	// Parcourir les entrées du répertoire
	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL) {
		// Vérifier si l'entrée est un fichier se terminant par ".cpt"
		int len = strlen(entry->d_name);
		if (len > 4 && strcmp(entry->d_name + len - 4, ".cpt") == 0) {
    			// Ouvrir le fichier
    			char file_path[260];
    			sprintf(file_path, "%s/%s", dir_name, entry->d_name);
			FILE* file = fopen(file_path, "r");
			if (file == NULL) {
				fprintf(stderr, "Erreur : impossible d'ouvrir le fichier %s\n", file_path);
				continue;
			}

			// Allouer de la mémoire pour la structure de comptine
			struct comptine* cpt = (struct comptine*) malloc(sizeof(struct comptine));
			if (cpt == NULL) {
				fprintf(stderr, "Erreur : impossible d'allouer de la mémoire pour la comptine\n");
				fclose(file);
				continue;
			}

			// Lire la première ligne du fichier comme titre
			char buffer[100];
			if (fgets(buffer, 100, file) == NULL) {
				fprintf(stderr, "Erreur : impossible de lire le titre du fichier %s\n", file_path);
				fclose(file);
				continue;
			}

			// Allouer de la mémoire pour la chaîne de caractères de titre et la copier
			cpt->titre = (char*) malloc((strlen(buffer) +sizeof(char)));
			if (cpt->titre == NULL) {
				fprintf(stderr, "Erreur : impossible d'allouer de la mémoire pour le titre\n");
				fclose(file);
				continue;
			}
			strcpy(cpt->titre, buffer);
					    
			cpt->nom_fichier = (char*) malloc((len - 3) * sizeof(char));
			if (cpt->nom_fichier == NULL) {
				 fprintf(stderr, "Erreur : impossible d'allouer de la mémoire pour le nom de fichier\n");
				 fclose(file);
				 continue;
			}

			// 2) Copier le nom de fichier sans l'extension .cpt
			strncpy(cpt->nom_fichier, entry->d_name, len - 4);
			cpt->nom_fichier[len - 4] = '\0';

			// 3) Ajouter le pointeur vers la comptine au tableau du catalogue
			cat->tab[cat->nb] = cpt;

			// 4) Incrémenter le nombre de fichiers comptine dans le catalogue
			cat->nb++;

			// Fermer le fichier
			fclose(file);
		}
	}

	// Fermer le répertoire
	closedir(dir);

	// Retourner le pointeur vers le catalogue
	return cat;
}
	
	


void liberer_catalogue(struct catalogue *c)
{
	int i;
	for(i=0; i < c->nb;i++){
		free(c->tab[i]->titre);
		free(c->tab[i]->nom_fichier);
	}
	free(c->tab);
	free(c);
}
	
		
	


