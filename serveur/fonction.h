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

extern unsigned long tailleMemoire;
extern unsigned long tailleMemoireThread;

typedef struct ArgumentThreadClient
{
    Client*        client;
    int            descripteurDeSocketServeur;
    uint8_t        indexClient;
    unsigned long  taille;
    uint8_t*       sortie;
    uint8_t*       sortieClient;
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
 * @brief fonction strEq, test légalité de 2 chaînes de caractères.
 * 
 * @param str1 La première chaîne à comparer.
 * @param str2 La chaîne à comparer.
 * 
 * @return 1 si les deux chaînes sont égales, 0 sinon.
 */
uint8_t strEq(const char* str1, const char* str2);

/**
 * @brief fonction printfSyslog, imprime la chaîne donnée sur la console et via le système de journalisation.
 * 
 * @param str La chaîne à imprimer.
 * @param logType Le type de log.
 */
void printfSyslog(const char* str, const uint8_t logType);

/**
 * @brief fonction configAddr, configuration adresse de la socket et la remplit avec le port et l'adresse du serveur.
 * 
 * @param addr : adresse de la socket
 * 
 * @return 0 si succès, -1 sinon
 */
int8_t configAddr(struct sockaddr_in *addr);


/**
 * @brief fonction verifJoinThread, vérifie la fin de fonctionnement du thread.
 * 
 * @param thread Le thread donné à vérifier.
 * @param index Index du tableau de thread si c'en est un.
 * 
 */
void verifJoinThread(pthread_t* thread, uint8_t index);

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

/**
 * @brief fonction tri_choix, tri du choix du client.
 * 
 * @param client : structure client
 * @param indexClient : index du client
 * @param choix : choix du client
 * @param sortie : sortie du programme
 */
void triChoix(
    Client* client, 
    uint8_t indexClient, 
    char* choix, 
    uint8_t* sortie, 
    uint8_t* sortieClient, 
    unsigned long taille_memoire, 
    int descripteurDeSocketServeur
);

#endif