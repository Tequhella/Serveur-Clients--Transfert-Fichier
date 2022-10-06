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


int main(int argc, char** argv)
{
    openlog("serveur", LOG_PID | LOG_CONS, LOG_LOCAL0);


    /* Etape 1 */
    /***********/
    
    struct sockaddr_in addr;
    /* Création de la socket */
    if (configAddr(&addr) == -1)
    {
        printfSyslog("Erreur de configuration de l'adresse", LOG_PERROR);
        return -1;
    }


    /* Etape 2 */
    /***********/
    int descripteurDeSocketServeur = socket(PF_INET, SOCK_STREAM, 0);
    if (descripteurDeSocketServeur < 0)
    {
        printfSyslog("Problèmes pour créer la socket", LOG_PERROR);
        return -1;
    }
    printfSyslog("Socket créé.\n", LOG_INFO);

    /* Etape 3 */
    /***********/
    if (bind(descripteurDeSocketServeur,(struct sockaddr *)&addr,sizeof(struct sockaddr_in)) < 0)
    {
        printfSyslog("Problèmes pour faire le bind", LOG_PERROR);
        return -1;
    }
    printfSyslog("Socket liée\n", LOG_INFO);

    /* Etape 4 */
    /***********/
    if (listen(descripteurDeSocketServeur, 1) < 0)
    {
        printfSyslog("Problèmes pour faire l'écoute", LOG_PERROR);
        return -1;
    }

    /* Etape 5 */
    /***********/
    // uint8_t taille      = 1; // taille du tableau client
    // uint8_t indexClient = 0; // index du client
    uint8_t sortie      = 1; // sortie du programme
    

    char buffer[LONGUEUR_BUFFER]; // tableau de buffer qui va être utilisé pour stocker les buffers des clients
    memset(buffer, 0, LONGUEUR_BUFFER); // on initialise le tableau de buffer à 0

    ArgumentThreadClient argumentThreadClient = {
        .client = NULL,
        .descripteurDeSocketServeur = descripteurDeSocketServeur,
        .indexClient = 0,
        .taille = 0,
        .sortie = &sortie,
        .sortieClient = NULL
    }; // structure qui va être utilisée comme argument pour lancer le thread client

    pthread_t threadConnexion; // thread qui va être créé pour la réception des connexions

    /* Création du thread de connexion */
    if (pthread_create(&threadConnexion, NULL, stockageClient, (void*)&argumentThreadClient) != 0)
    {
        printfSyslog("Erreur lors de la création du thread de connexion.\n", LOG_PERROR);
        return -1;
    }
    printfSyslog("Thread de connexion créé.\n", LOG_INFO);

    /* Etape 6 */
    /***********/
    verifJoinThread(&threadConnexion, 0);

    close(descripteurDeSocketServeur);
    closelog();

    pthread_exit(NULL);

    return 0;
}