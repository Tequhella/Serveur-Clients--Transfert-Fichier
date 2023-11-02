/**
 * @file serveur.c
 * @author Lilian CHARDON, Andréas Castello (lilian.chardon@ynov.com, andreas.castello@ynov.com)
 * @brief Gestionnaire de transfert de fichier et de stockage en TCP/IP
 * @version 0.1
 * @date 19-10-2023
 * 
 * @copyright Copyright (c) 2022
 * 
 */


/****************************/
/* Socket serveur itérative */
/****************************/

#include "reseau.h"
#include "baseFonction.h"
#include "gestionClient.h"


int main(int argc, char** argv)
{
    /**
     * This function initializes the syslog logging facility with the given parameters.
     * The first parameter is the name of the program, the second parameter specifies that 
     * the process ID should be included in each log message, and the third parameter specifies 
     * that log messages should be written to the system console.
     */
    openlog("serveur", LOG_PID | LOG_CONS, LOG_LOCAL0);


    /* Etape 1 */
    /***********/
    
    /*
     * Creates a socket and configures its address.
     * 
     * @param addr A pointer to a sockaddr_in struct to be configured.
     * @return Returns -1 if there was an error configuring the address, otherwise returns 0.
     */
    struct sockaddr_in addr[NB_CLIENT_MAX] = {0};
    if (configAddr(addr) == -1)
    {
        printfSyslog("Erreur de configuration de l'adresse", LOG_PERROR);
        return -1;
    }

    int descripteurDeSocketServeur[NB_CLIENT_MAX] = {0};

    /* Etape 2, 3 & 4 */
    /******************/
    if (configSocket(descripteurDeSocketServeur, addr) == -1)
    {
        return -1;
    }    

    /* Etape 5 */
    /***********/
    char buffer[LONGUEUR_BUFFER]; // tableau de buffer qui va être utilisé pour stocker les buffers des clients
    memset(buffer, 0, LONGUEUR_BUFFER); // on initialise le tableau de buffer à 0

    ArgumentThreadClient argumentThreadClient = {
        .client = NULL,
        .descripteurDeSocketServeur = descripteurDeSocketServeur,
        .indexClient = 0,
        .nbClient = 0,
        .sortie = NULL,
        .sortieClient = NULL,
        .adresseDuServeur = addr
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

    close(*descripteurDeSocketServeur);
    closelog();

    pthread_exit(NULL);

    return 0;
}