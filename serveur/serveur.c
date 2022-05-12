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
    openlog("serveur", LOG_PID | LOG_CONS, LOG_LOCAL0);


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
        perror("Problemes pour créer la socket.\n");
        syslog(LOG_ERR, "Problemes pour créer la socket.\n");
        return -1;
    }
    printf("Socket créé.\n");
    syslog(LOG_INFO, "Socket créé.\n");
    /* Etape 3 */
    /***********/
    if (bind(descripteurDeSocketServeur,(struct sockaddr *)&addr,sizeof(struct sockaddr_in)) < 0)
    {
        perror("Problemes pour faire le bind.\n");
        syslog(LOG_ERR, "Problemes pour faire le bind.\n");
        return -1;
    }
    printf("Socket liée\n");
    syslog(LOG_INFO, "Socket liée\n");
    /* Etape 4 */
    /***********/
    if (listen(descripteurDeSocketServeur, 1) < 0)
    {
        perror("Problemes pour faire l'écoute.\n");
        syslog(LOG_ERR, "Problemes pour faire l'écoute.\n");
        return -1;
    }

    Client* client = malloc (sizeof(Client)); // client qui va être créé à chaque fois qu'un client se connecte
    if (!client)
    {
        perror("Erreur d'allocation de mémoire du client.\n");
        syslog(LOG_ERR, "Erreur d'allocation de mémoire du client.\n");
        return -1;
    }
    uint8_t taille      = 1; // taille du tableau client
    uint8_t indexClient = 0; // index du client
    uint8_t sortie      = 1; // sortie du programme
    char buffer[LONGUEUR_BUFFER]; // tableau de buffer qui va être utilisé pour stocker les buffers des clients
    memset(buffer, 0, LONGUEUR_BUFFER); // on initialise le tableau de buffer à 0
    
    stockage_client(client, descripteurDeSocketServeur); // stockage du client

    /* Etape 5 */
    /***********/
    while (sortie == 1)
    {
        taille      = sizeof(client) / sizeof(Client);
        indexClient = taille - 1;
        
        if (client->descripteurDeSocketClient != 0)
        {
            reception_client(client, &taille, buffer); // reception du message du client
        }
        
        tri_choix(client, indexClient, buffer, &sortie); // tri du client
        
    }
    /* Il ferme le socket de chaque client et le socket du serveur. */
    close(client->descripteurDeSocketClient);
    close(descripteurDeSocketServeur);
    closelog();

    /* Il libère la mémoire allouée au client et au tampon. */
    free (client);
    client = NULL;

    return 0;
}