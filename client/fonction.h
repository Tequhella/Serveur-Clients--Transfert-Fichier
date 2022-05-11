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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <syslog.h>

#include "../global.h"

// -lpthread
// -D_REENTRANT

// C'est une structure qui contient le descripteur de socket du Serveur, l'adresse du Serveur et la longueur de l'adresse du Serveur.
typedef struct Serveur
{
    int                descripteurDeSocketServeur;
    struct sockaddr_in addresseDuServeur;
    unsigned int       longueurDeAdresseDuServeur;
}
Serveur;

/**
 * @brief fonction str_eq, test légalité de 2 chaînes de caractères.
 * 
 * @param str1 La première chaîne à comparer.
 * @param str2 La chaîne à comparer.
 * 
 * @return 1 si les deux chaînes sont égales, 0 sinon.
 */
uint8_t str_eq(const char* str1, const char* str2);

/**
 * @brief fonction config_addr, configuration adresse de la socket et la remplit avec le port et l'adresse du serveur.
 * 
 * @param addr : adresse de la socket
 * 
 * @return 0 si succès, -1 sinon
 */
int8_t config_addr(struct sockaddr_in* addr);

/**
 * @brief fonction reception_serveur, reception du message du serveur.
 * 
 * @param serveur : structure serveur
 * @param buffer : buffer de reception
 */
void reception_serveur(Serveur* serveur, char* buffer);

#endif