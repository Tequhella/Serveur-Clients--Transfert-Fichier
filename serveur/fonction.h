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

#define LONGUEUR_BUFFER 1024

#include "../global.h"

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

typedef struct ArgumentThreadClient
{
    Client*  client;
    int      descripteurDeSocketServeur;
    uint8_t  indexClient;
    uint8_t  taille;
    uint8_t* sortie;
}
ArgumentThreadClient;

/**
 * @brief fonction itoa, convertit un entier en une chaîne de caractères.
 * 
 * @param val : entier à convertir.
 * @param base : base de la conversion.
 * @return char* : chaîne de caractères correspondant à l'entier.
 */
char* itoa(int val, int base);

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
 * @param argumentThread : structure contenant le client et le descripteur de socket du serveur.
 * (client : structure client)
 * (longueurDeAdresseDuClient : longueur de l'adresse du client)
 */
void* stockage_client(void* argumentThread);

/**
 * @brief Reçoit la requête du client et l'envoie à la fonction qui la traitera
 * 
 * @param argumentThread structure contenant le client, l'index du client, la taille et du stock de client et le buffer.
 * (client : structure client)
 * (indexClient : index du client)
 * (taille : taille du tableau)
 * (sortie : sortie du serveur)
 */
void* reception_client(void* argumentThread);

/**
 * @brief fonction tri_choix, tri du choix du client.
 * 
 * @param client : structure client
 * @param indexClient : index du client
 * @param choix : choix du client
 * @param sortie : sortie du programme
 */
void tri_choix(Client* client, uint8_t indexClient, char* choix, uint8_t* sortie);

#endif