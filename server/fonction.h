/**
 * @file fonction.h
 * @author Lilian CHARDON, Andréas Castello (lilian.chardon@ynov.com, andreas.castello@ynov.com)
 * @brief 
 * @version 0.1
 * @date 05-05-2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef FONCTION_H
#define FONCTION_H

#define NULL (void*)0
#define LONGUEUR_BUFFER 1024

#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <syslog.h>

// -lpthread
// -D_REENTRANT

// C'est une structure qui contient le descripteur de socket du client, l'adresse du client et la longueur de l'adresse du client.
typedef struct Client
{
    int                descripteurDeSocketClient;
    struct sockaddr_in adresseDuClient;
    unsigned int       longueurDeAdresseDuClient;
}
Client;

/**
 * @brief fonction str_eq, test légalité de 2 chaînes de caractères.
 * 
 * @param str1 La première chaîne à comparer.
 * @param str2 La chaîne à comparer.
 * 
 * @return 1 si les deux chaînes sont égales, 0 sinon.
 */
uint8_t str_eq(const char *str1, const char *str2);

/**
 * @brief fonction config_addr, configuration adresse de la socket et la remplit avec le port et l'adresse du serveur.
 * 
 * @param addr : adresse de la socket
 * 
 * @return 0 si succès, -1 sinon
 */
int8_t config_addr(struct sockaddr_in *addr);

/**
 * @brief fonction stockage_client, stockage du client dans la structure Client.
 * 
 * @param client : structure client
 * @param longueurDeAdresseDuClient : longueur de l'adresse du client
 */
void stockage_client(Client *client, int descripteurDeSocketServeur);

/**
 * @brief fonction reception_client, reception du message du client.
 * 
 * @param client : structure client
 * @param taille : taille du tableau
 * @param buffer : message du client
 * 
 * @return int : 0 si succès, -1 sinon
 */
int8_t reception_client(Client *client, uint8_t* taille, char** buffer);

#endif