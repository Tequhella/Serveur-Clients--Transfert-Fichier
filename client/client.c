/**
 * @file client.c
 * @author Lilian CHARDON, Andréas Castello (lilian.chardon@ynov.com, andreas.castello@ynov.com)
 * @brief Gestionnaire de transfert de fichier et de stockage en TCP/IP.
 * @version 0.1
 * @date 09-05-2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/*****************/
/* Socket client */
/*****************/
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "fonction.h"


int main(int argc, char **argv)
{
    /* Déclaration de l'addresse IP, du numéro de port et du protocole */
    /******************************************************************/
    struct sockaddr_in addresseDuServeur;
    if (config_addr(&addresseDuServeur) == -1)
    {
        perror("Erreur de configuration de l'adresse.\n");
        return -1;
    }
    
    Serveur serveur =
    {
        .descripteurDeSocketServeur = socket(PF_INET, SOCK_STREAM, 0),
        .addresseDuServeur          = addresseDuServeur,
        .longueurDeAdresseDuServeur = sizeof(addresseDuServeur)
    };
    if (serveur.descripteurDeSocketServeur < 0)
    {
        printf("Problemes pour creer la socket");
        return -1;
    }
    

    /* Affichage des informations de connexion */
    /*******************************************/
    printf("Connexion vers la machine ");
    unsigned char *addresseDuServeurIP = (unsigned char *)&(addresseDuServeur.sin_addr.s_addr);
    printf("%d.", *(addresseDuServeurIP));
    printf("%d.", *(addresseDuServeurIP + 1));
    printf("%d.", *(addresseDuServeurIP + 2));
    printf("%d", *(addresseDuServeurIP + 3));
    printf(" sur le port %u \n", PORT);
    
    printf("socket cree\n");
    /* Connexion de la socket au serveur */
    /*************************************/
    if (connect(serveur.descripteurDeSocketServeur,(struct sockaddr *)&addresseDuServeur,sizeof(addresseDuServeur)) < 0)
    {
        printf("Problemes pour se connecter au serveur");
        return -1;
    }
    printf("socket connectee\n");

    /* Lecture la réponse grâce à recv */
    /***********************************/
    char buffer[LONGUEUR_BUFFER];
    memset(buffer, '\0', LONGUEUR_BUFFER);
    int nbOctetsRecus = 0;
    uint8_t sortie = 1;

    reception_serveur(&serveur, buffer);

    char mot[20];
    char choixFichier[20];

    //system("clear");
    printf("Voici les commandes que vous pouvez utiliser : \nls\n%s\n%s\n%s\n%s\n%s\n", requeteCd, requeteSend, requeteHelp, requeteExit, requeteShutdown);

    while (sortie == 1)
    {
        printf("Veuillez entrer votre commande : ");
        scanf("%s", mot);

        if (str_eq(mot, requeteSend))
        {
            // On envoi la requete send pour que le serveur se mette en condition de recevoir un fichier
            printf("Envoi de la requete : %s \n", requeteSend);
            send(
                serveur.descripteurDeSocketServeur,
                requeteSend,
                sizeof(requeteSend),
                0
            );
            
            // On vérifie que le serveur nous a bien envoyé une réponse
            reception_serveur(&serveur, buffer);
            
            if (str_eq(buffer, "Requete lue."))
            {
                printf("Veuillez entrer le nom du fichier que vous voulez envoyer : ");
                scanf("%s", choixFichier);
                /* On envoie le nom du fichier au serveur. */
                send(
                    serveur.descripteurDeSocketServeur,
                    choixFichier,
                    sizeof(choixFichier),
                    0
                );
                strcat(choixFichier, ".txt");
                FILE* fichier = fopen(choixFichier, "r");
                if (fichier)
                {
                    /* On envoie le contenu au serveur. */
                    fgets(buffer, LONGUEUR_BUFFER, fichier);
                    send(
                        serveur.descripteurDeSocketServeur,
                        buffer,
                        LONGUEUR_BUFFER,
                        0
                    );
                    fclose(fichier);
                }
                else
                {
                    printf("Le fichier n'existe pas.\n");
                }
                reception_serveur(&serveur, buffer);
                
            }
            else
            {
                printf("Erreur serveur : impossible de recevoir de fichier \n");
            }
        }
        else if (str_eq(mot, "ls"))
        {
            // On envoi la requete ls pour que le serveur liste les fichiers.
            printf("Envoi de la requete : %s \n", requeteLs);
            send(
                serveur.descripteurDeSocketServeur,
                requeteLs,
                sizeof(requeteLs),
                0
            );
            reception_serveur(&serveur, buffer);
            
            if (str_eq(buffer, "Requete lue."))
            {
                reception_serveur(&serveur, buffer);
                printf("\nReponse du serveur : \n %s \n", buffer);
            }
        }
        else if (str_eq(mot, requeteDl))
        {
            // On envoi la requete dl pour que le serveur nous envoie un fichier.
            printf("Envoi de la requete : %s \n", requeteDl);
            send(
                serveur.descripteurDeSocketServeur,
                requeteDl,
                sizeof(requeteDl),
                0
            );
            reception_serveur(&serveur, buffer);
            
            if (str_eq(buffer, "Requete lue."))
            {
                printf("Veuillez entrer le nom du fichier que vous voulez recevoir : ");
                scanf("%s", choixFichier);
                send(
                    serveur.descripteurDeSocketServeur,
                    choixFichier,
                    sizeof(choixFichier),
                    0
                );
                reception_serveur(&serveur, buffer);
                
                if (str_eq(buffer, "Le dossier existe bien."))
                {
                    reception_serveur(&serveur, buffer);
                    
                    if (str_eq(buffer, "Le fichier existe bien, envoi en cours..."))
                    {
                        // On reçoit le nom du fichier
                        reception_serveur(&serveur, buffer);
                        
                        FILE* fichier = fopen(buffer, "w");
                        if (fichier)
                        {
                            reception_serveur(&serveur, buffer);
                            fputs(buffer, fichier);
                            fclose(fichier);
                            printf("Fichier recu avec succes \n");
                        }
                        else
                        {
                            printf("Erreur d'ouverture du fichier \n");
                        }
                    }
                }
                else
                {
                    printf("Erreur : le fichier n'existe pas \n");
                }
            }
            else
            {
                printf("Erreur serveur : impossible de recevoir de fichier \n");
            }
        }
        else if (str_eq(mot, requeteHelp))
        {
            printf("Voici les commandes que vous pouvez utiliser : \nls\n%s\n%s\n%s\n%s\n", requeteCd, requeteSend, requeteHelp, requeteExit);
        }
        else if (str_eq(mot, requeteExit))
        {
            printf("Envoi de la requete : %s \n", requeteShutdown);
            send(
                serveur.descripteurDeSocketServeur,
                requeteExit,
                sizeof(requeteExit),
                0
            );
            reception_serveur(&serveur, buffer);
            
            if (str_eq(buffer, "Requete lue."))
            {
                sortie = 0;
            }
            else
            {
                printf("Erreur serveur : impossible de vous deconnecter de manière normale \n");
            }
        }
        else if (str_eq(mot, requeteShutdown) || str_eq(mot, "sh"))
        {
            printf("Envoi de la requete : %s \n", requeteShutdown);
            send(
                serveur.descripteurDeSocketServeur,
                requeteShutdown,
                sizeof(requeteShutdown),
                0
            );
            reception_serveur(&serveur, buffer);
            
            if (str_eq(buffer, "Requete lue."))
            {
                printf("Shutdown reussi \n");
                sortie = 0;
            }
            else
            {
                printf("Erreur shutdown \n");
            }
        }
        else if (str_eq(mot, requeteCd))
        {
            printf("Envoi de la requete : %s \n", requeteCd);
            send(
                serveur.descripteurDeSocketServeur,
                requeteCd,
                sizeof(requeteCd),
                0
            );
            reception_serveur(&serveur, buffer);
            
            if (str_eq(buffer, "Requete lue."))
            {
                printf("Veuillez entrer le nom du dossier que vous voulez acceder : ");
                scanf("%s", choixFichier);
                send(
                    serveur.descripteurDeSocketServeur,
                    choixFichier,
                    sizeof(choixFichier),
                    0
                );
                reception_serveur(&serveur, buffer);
                
                if (str_eq(buffer, "Ok."))
                {
                    printf("Accès au dossier reussi \n");
                }
                else
                {
                    printf("Erreur accès au dossier \n");
                }
            }
            else
            {
                printf("Erreur serveur : impossible de changer de dossier \n");
            }
        }
        else
        {
            printf("Commande inconnue \n");
        }
    }
    
    printf("\n\nFIN\n\n");

    /* Fermeture de la connexion */
    /*****************************/
    close(serveur.descripteurDeSocketServeur);
    
    return 0;
}