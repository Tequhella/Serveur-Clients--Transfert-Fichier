/**
 * @file main.c
 * @author Lilian CHARDON, Andréas Castello (lilian.chardon@ynov.com, andreas.castello@ynov.com)
 * @brief Gestionnaire de transfert de fichier et de stockage en TCP/IP
 * @version 0.1
 * @date 10/04/2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */


/****************************/
/* Socket serveur itérative */
/****************************/
#include "fonction.h"

/**
 * @brief Fonction de création de socket
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char **argv)
{
    /* Etape 1 */
    /***********/
    
    struct sockaddr_in addr;
    /* Création de la socket */
    if (config_addr(&addr) == -1)
    {
        perror("Erreur de configuration de l'adresse.\n");
        return -1;
    }


    /* Etape 2 */
    /***********/
    int descripteurDeSocketServeur = socket(PF_INET, SOCK_STREAM, 0);
    if (descripteurDeSocketServeur < 0)
    {
        printf("Problemes pour créer la socket.\n");
        return -1;
    }
    printf("Socket créé.\n");
    /* Etape 3 */
    /***********/
    if (bind(descripteurDeSocketServeur,(struct sockaddr *)&addr,sizeof(struct sockaddr_in)) < 0)
    {
        printf("Problemes pour faire le bind.\n");
        return -1;
    }
    printf("Socket liée\n");
    /* Etape 4 */
    /***********/
    if (listen(descripteurDeSocketServeur, 1) < 0)
    {
        printf("Problemes pour faire l'écoute.\n");
        return -1;
    }

    Client* client      = malloc (sizeof(Client)); // client qui va être créé à chaque fois qu'un client se connecte
    uint8_t indexClient = sizeof (client) / sizeof(Client) - 1; // taille du tableau client
    char* buffer        = malloc (LONGUEUR_BUFFER); // buffer qui va être utilisé pour lire les données du client

    pthread_t thread_connexion; // thread qui va gérer la connexion des clients
    pthread_t thread_reception; // thread qui va gérer la réception des données des clients
    pthread_t thread_envoi; // thread qui va gérer l'envoi des données des clients
    
    if (pthread_create(&thread_connexion, NULL, stockage_client, (client, descripteurDeSocketServeur)) != 0)
    {
        printf("Problèmes pour créer le thread de connexion.\n");
        return -1;
    }

    /* Etape 5 */
    /***********/
    while (1)
    {
        indexClient = sizeof(client) / sizeof(Client) - 1;
        if (pthread_create(&thread_reception, NULL, reception_client, (client, indexClient + 1, buffer)) != 0)
        {
            printf("Problèmes pour créer le thread de réception.\n");
            return -1;
        }
        char* buffer = realloc(buffer, LONGUEUR_BUFFER * indexClient);
        for (uint8_t i = 0; i < indexClient; i++)
        {
            if (client[i].descripteurDeSocketClient != 0)
            {
                
            }
            /* Etape 6 */
            /***********/
            unsigned int nbCaracteres;
            unsigned int i;
            char buffer[1024];
            memset(buffer, 0, 1024);
            printf("\nLecture de la requete : \n");
            do
            {
                nbCaracteres = recv(client[i].descripteurDeSocketClient, buffer, 1024, 0);
                for (i = 0; i < nbCaracteres; i++)
                    printf("%c", buffer[i]);
            } while (nbCaracteres == 1024);

            /* Etape 7 */
            /***********/
            char* reponse = NULL;
            /* Envoi de la réponse au client. */
            printf("\nEcriture de la reponse : %s\n", reponse);
            send(client[i].descripteurDeSocketClient, reponse, sizeof(reponse), 0);
        }
        

        
        /* Etape 8 */
        /***********/
    }
    pthread_exit(NULL);
    close(descripteurDeSocketServeur);
    for (uint8_t i = 0; i < sizeof(client) / sizeof(Client); i++)
    {
        close(client[i].descripteurDeSocketClient);
    }
    free (client);

    return 0;
}