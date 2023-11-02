/**
 * @file gestionClient.h
 * @author Lilian CHARDON, Andréas Castello (lilian.chardon@ynov.com, andreas.castello@ynov.com)
 * @brief 
 * @version 0.1
 * @date 19-10-2023
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef GESTION_CLIENT_H
#define GESTION_CLIENT_H

#define LONGUEUR_BUFFER 1024

#include "global.h"

// C'est une structure qui contient le descripteur de socket du client, l'adresse du client et la longueur de l'adresse du client.
typedef struct Client
{
    int                descripteurDeSocketClient;
    struct sockaddr_in adresseDuClient;
    unsigned int       longueurDeAdresseDuClient;
}
Client;

extern unsigned long tailleMemoire;
extern unsigned long tailleMemoireThread;

typedef struct ArgumentThreadClient
{
    Client*             client;
    int*                descripteurDeSocketServeur;
    uint8_t             indexClient;
    unsigned long       nbClient;
    uint8_t*            sortie;
    uint8_t*            sortieClient;
    struct sockaddr_in* adresseDuServeur;
}
ArgumentThreadClient;


/**
 * @brief fonction stockageClient, stockage du client dans la structure Client.
 * 
 * @param argumentThread : structure contenant le client et le descripteur de socket du serveur.
 * (client : structure client)
 * (longueurDeAdresseDuClient : longueur de l'adresse du client)
 */
void* stockageClient(void* argumentThread);

/**
 * @brief Reçoit la requête du client et l'envoie à la fonction qui la traitera
 * 
 * @param argumentThread structure contenant le client, l'index du client, la taille et du stock de client et le buffer.
 * (client : structure client)
 * (indexClient : index du client)
 * (taille : taille du tableau)
 * (sortie : sortie du serveur)
 */
void* receptionClient(void* argumentThread);


#endif // GESTION_CLIENT_H