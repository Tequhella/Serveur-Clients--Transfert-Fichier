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

#include "global.h"
#include "fonction.h"


int main(int argc, char **argv)
{
    /* Déclaration de l'addresse IP, du numéro de port et du protocole */
    /******************************************************************/
    struct sockaddr_in addresseDuClient;
    if (config_addr(&addresseDuClient) == -1)
    {
        perror("Erreur de configuration de l'adresse.\n");
        return -1;
    }
    

    /* Affichage des informations de connexion */
    /*******************************************/
    printf("Connexion vers la machine ");
    unsigned char *addresseDuClientIP = (unsigned char *)&(addresseDuClient.sin_addr.s_addr);
    printf("%d.", *(addresseDuClientIP));
    printf("%d.", *(addresseDuClientIP + 1));
    printf("%d.", *(addresseDuClientIP + 2));
    printf("%d", *(addresseDuClientIP + 3));
    printf(" sur le port %u \n", PORT);
    
    /* Creation de la socket */
    /*************************/
    int descripteurDeSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (descripteurDeSocket < 0)
    {
        printf("Problemes pour creer la socket");
        return -1;
    }
    printf("socket cree\n");
    /* Connexion de la socket au serveur */
    /*************************************/
    if (connect(descripteurDeSocket,(struct sockaddr *)&addresseDuClient,sizeof(addresseDuClient)) < 0)
    {
        printf("Problemes pour se connecter au serveur");
        return -1;
    }
    printf("socket connectee\n");
    /* Envoi la requête grâce à send */
    /*********************************/
    printf("\nEnvoi de la requete : %s \n", requete);
    send(descripteurDeSocket, requete, sizeof(requete), 0);
    /* Lecture la réponse grâce à recv */
    /***********************************/
    char buffer[LONGUEUR_BUFFER];
    char* commande;
    uint8_t sortie = 1;
    unsigned int nbCaracteres;
    unsigned int i;

    while (sortie == 1)
    {
        

    }
    

    printf("\nReception de la reponse : \n");
    do
    {
        nbCaracteres = recv(descripteurDeSocket, buffer, 1024, 0);
        for (i = 0; i < nbCaracteres; i++)
        {
            printf("%c", buffer[i]);
        }
    } while (nbCaracteres == 1024);
        char* temp = buffer;
            if (file)
            {
                fputs (temp, file);
            }
            else
            {
                printf ("Le fichier %s n'a pas pu être ouvert.\n", nomFichier);
            }
            fclose (file);
    printf("\n\nFIN\n\n");
    /* Fermeture de la connexion */
    /*****************************/
    close(descripteurDeSocket);
    return 0;
}