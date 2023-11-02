/**
 * @file reseau.c
 * @author Lilian CHARDON, Andréas Castello (lilian.chardon@ynov.com, andreas.castello@ynov.com)
 * @brief 
 * @version 0.1
 * @date 19-10-2023
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "reseau.h"

uint8_t indexTab4 = 0;
int port = 2000;


/**
 * Configures the given socket address structure with an available port and INADDR_ANY.
 * 
 * @param addr The socket address structure to configure.
 * @return 0 if successful, -1 if an error occurred.
 */
int8_t configAddr(struct sockaddr_in* addr)
{
    // check if port is already used
    while (portUsed(port))
        port++;

    addr->sin_family = PF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_ANY;

    port++;

    return 0;
}

/**
 * @brief Configure la socket pour écouter sur l'addresse spécifiée
 * 
 * @param descripteurDeSocketServeur le descripteur de la socket à configurer
 * @param addr l'addresse à écouter
 * 
 * @return 0 si succès, -1 sinon
 */
int8_t configSocket(int* descripteurDeSocketServeur, struct sockaddr_in* addr)
{
    /* Etape 2 */
    /***********/
    /**
     * Creates a socket using the specified protocol family, socket type and protocol.
     * 
     * @param PF_INET   Protocol family for IPv4.
     * @param SOCK_STREAM   Provides sequenced, reliable, two-way, connection-based byte streams.
     * @param 0   Protocol value for IP.
     * 
     * @return Returns a non-negative integer representing the socket file descriptor on success, and -1 on error.
     */
    
    descripteurDeSocketServeur[indexTab4] = socket(PF_INET, SOCK_STREAM, 0);
    if (descripteurDeSocketServeur < 0)
    {
        printfSyslog("Problèmes pour créer la socket", LOG_PERROR);
        return -1;
    }
    printfSyslog("Socket créé.\n", LOG_INFO);
    
    /* Etape 3 */
    /***********/
    /**
     * Binds the server socket to the specified address and port.
     * 
     * @param descripteurDeSocketServeur The server socket descriptor.
     * @param addr The address and port to bind to.
     * @return Returns -1 if there was an error binding the socket, otherwise returns 0.
     */
    if (bind(descripteurDeSocketServeur[indexTab4],(struct sockaddr*)addr + indexTab4, sizeof(struct sockaddr_in)) < 0)
    {
        printfSyslog("Problèmes pour faire le bind", LOG_PERROR);
        return -1;
    }
    printfSyslog("Socket liée\n", LOG_INFO);

    /* Etape 4 */
    /***********/
    /**
     * This code block listens for incoming connections on a server socket.
     * If there is an error while listening, an error message is logged and the function returns -1.
     */
    if (listen(descripteurDeSocketServeur[indexTab4], 1) < 0)
    {
        printfSyslog("Problèmes pour faire l'écoute", LOG_PERROR);
        return -1;
    }

    /**
     * This line of code increments the indexTab4 variable by 1, unless it has already reached the maximum value of NB_CLIENT_MAX - 1.
     * If it has reached the maximum value, it remains unchanged.
     */
    if (indexTab4 == NB_CLIENT_MAX - 1) /*--->*/ indexTab4 = NB_CLIENT_MAX - 1;
    else /*---------------------------------->*/ indexTab4++;

    return 0;
}

/**
 * @brief Vérifie si le port spécifié est déjà utilisé
 * 
 * @param port le port à vérifier
 * 
 * @return 1 si le port est utilisé, 0 sinon
 */
uint8_t portUsed(int port)
{
    struct sockaddr_in addr;
    int descripteurDeSocketServeur = socket(PF_INET, SOCK_STREAM, 0);
    if (descripteurDeSocketServeur < 0)
    {
        printfSyslog("Problèmes pour créer la socket", LOG_PERROR);
        return -1;
    }
    addr.sin_family = PF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(descripteurDeSocketServeur,(struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0)
    {
        return 1;
    }
    close(descripteurDeSocketServeur);
    return 0;
}