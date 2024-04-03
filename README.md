Mini-projet Wordle en Systèmes et Réseaux

Ce projet académique a été réalisé dans le cadre du cours de Systèmes et Réseaux  sous la supervision du professeur Pierre Rousselin.

Description du Projet

Le jeu Wordle est un jeu de lettres en ligne très populaire créé par Josh Wardle, qui a été acquis par le New York Times en janvier 2022. L'objectif de ce projet était de transformer une version du jeu Wordle en français, fonctionnant sur une seule machine avec un seul processus, en un jeu en réseau.

Pour voir à quoi ressemble le jeu, vous pouvez visiter ce lien.

Instructions
    Le code complet du client et du serveur, avec un Makefile mis à jour pour compiler avec la commande $ make.
    Chaque fichier .c ou .h commencera par :

    Un rapport concis et clair au format texte ou PDF, contenant les éléments spécifiés plus loin.

Protocole WRDLP

Au niveau de la couche transport, TCP est utilisé car la réactivité n'est pas critique dans ce jeu. Le protocole applicatif développé pour ce projet est appelé Wordle Protocol (WRDLP), associé au port 4242.

Voici quelques conseils et consignes pour la réalisation du projet :

    Le client transmet chaque proposition de l'utilisateur et le serveur répond en indiquant pour chaque lettre si elle est présente et au bon endroit, présente mais au mauvais endroit, ou absente du mot à trouver.
    Il est important de bien réfléchir à la répartition des tâches entre le serveur et le client.
    Le code doit être propre, bien commenté et correctement indenté.
    Les tests doivent être réalisés pour identifier les éventuels dysfonctionnements et les solutions proposées doivent être documentées dans le rapport.

Fonctionnalités Implémentées

    Le serveur est capable d'accepter une connexion d'un client, de choisir un mot au hasard et de permettre au client de jouer une partie complète.
    Le client peut demander une connexion à un serveur WRDLP en fournissant son adresse IP comme argument et permet à l'utilisateur de jouer une partie en communiquant avec le serveur.
    Le protocole WRDLP est décrit de manière détaillée dans le rapport.
    Les envois et réceptions de messages sont rigoureusement gérés.

Rapport

Le rapport d'environ une ou deux pages contient :

    Une description détaillée et claire du protocole WRDLP.
    Une explication des fonctionnalités implémentées avec des exemples pour illustrer l'implémentation.
    Les problèmes rencontrés et les tentatives de solutions.
    Le cas échéant, les ressources externes utilisées, avec leurs références.
    Toute assistance d'un autre étudiant, le cas échéant.

Conclusion

Ce projet permet de mettre en pratique les concepts abordés dans le cours de Systèmes et Réseaux et offre la possibilité d'explorer des problématiques liées aux jeux en réseau et à la communication entre client et serveur.


