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

    Client* client = malloc (sizeof(Client)); // client qui va être créé à chaque fois qu'un client se connecte
    if (!client)
    {
        perror("Erreur d'allocation de mémoire du t.\n");
        return -1;
    }
    uint8_t taille      = 1; // taille du tableau client
    uint8_t indexClient = 0; // index du client
    uint8_t sortie      = 1; // sortie du programme
    char** tabBuffer = malloc (indexClient); // tableau de buffer qui va être utilisé pour stocker les buffers des clients
    if (!tabBuffer)
    {
        perror("Erreur d'allocation de mémoire.\n");
        return -1;
    }
    memset(tabBuffer[0], 0, LONGUEUR_BUFFER); // on initialise le tableau de buffer à 0

    pthread_t thread_connexion; // thread qui va gérer la connexion des clients
    pthread_t thread_reception; // thread qui va gérer la réception des données des clients
    pthread_t thread_envoi; // thread qui va gérer l'envoi des données des clients
    
    if (pthread_create(&thread_connexion, NULL, stockage_client, (client, descripteurDeSocketServeur)) != 0)
    {
        printf("Problèmes pour créer le thread de connexion.\n");
        return -1;
    }

    if (client[0].descripteurDeSocketClient != 0)
    {
        if (pthread_create(&thread_reception, NULL, reception_client, (client, taille, tabBuffer)) != 0)
        {
            printf("Problèmes pour créer le thread de réception.\n");
            return -1;
        }
    }

    /* Etape 5 */
    /***********/
    while (sortie == 1)
    {
        taille      = sizeof(client) / sizeof(Client);
        indexClient = taille - 1;
        
        
        tabBuffer = realloc(tabBuffer, taille);
        if (!tabBuffer)
        {
            printf("Problèmes de reallocation du buffer.\n");
            return -1;
        }
        for (uint8_t i = 0; i < taille; i++)
        {
            if (str_eq(tabBuffer[i], "FIN"))
            {
                indexClient = i;
                memset(tabBuffer[i], 0, LONGUEUR_BUFFER);
                sortie--;
            }
            
        }
        
    }
    /* On termine le thread appelant. */
    pthread_exit(NULL);

    /* Il ferme le socket de chaque client et le socket du serveur. */
    for (uint8_t i = 0; i < sizeof(client) / sizeof(Client); i++)
    {
        close(client[i].descripteurDeSocketClient);
    }
    close(descripteurDeSocketServeur);

    /* Il libère la mémoire allouée au client et au tampon. */
    free (client);
    free (tabBuffer);
    client    = NULL;
    tabBuffer = NULL;

    return 0;
}