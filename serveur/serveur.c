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
int main(int argc, char** argv)
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

    Client* client = malloc (sizeof(Client)); // client créé et sera agrandit à chaque fois qu'un client se connecte
    if (!client)
    {
        perror("Erreur d'allocation de mémoire du client.\n");
        syslog(LOG_ERR, "Erreur d'allocation de mémoire du client.\n");
        return -1;
    }
    client->descripteurDeSocketClient = 0;

    uint8_t taille      = 1; // taille du tableau client
    uint8_t indexClient = 0; // index du client
    uint8_t sortie      = 1; // sortie du programme

    char buffer[LONGUEUR_BUFFER]; // tableau de buffer qui va être utilisé pour stocker les buffers des clients
    memset(buffer, 0, LONGUEUR_BUFFER); // on initialise le tableau de buffer à 0

    ArgumentThreadClient argumentThreadClient = {
        .client = client,
        .descripteurDeSocketServeur = descripteurDeSocketServeur,
        .indexClient = 0,
        .taille = 0,
        .sortie = &sortie
    }; // structure qui va être utilisée comme argument pour lancer le thread client

    pthread_t thread_connexion; // thread qui va être créé pour la réception des connexions

    /* Création du thread de connexion */
    if (pthread_create(&thread_connexion, NULL, stockage_client, (void*)&argumentThreadClient) != 0)
    {
        perror("Erreur lors de la création du thread de connexion.\n");
        syslog(LOG_ERR, "Erreur lors de la création du thread de connexion.\n");
        return -1;
    }
    printf("Thread de connexion créé.\n");
    syslog(LOG_INFO, "Thread de connexion créé.\n");

    /* Etape 5 */
    /***********/
    while (sortie == 1)
    {
        
        
    }
    pthread_exit(NULL); // sortie du thread

    /* Recalcul de la taille du tableau client. */
    taille = sizeof(client) / sizeof(Client);

    /* Ferme le socket de chaque client et le socket du serveur. */
    for (int i = 0; i < taille; i++)
    {
        close(client[i].descripteurDeSocketClient);
    }
    close(descripteurDeSocketServeur);
    closelog();

    /* Libère la mémoire allouée au client. */
    free(client);
    client = NULL;

    return 0;
}