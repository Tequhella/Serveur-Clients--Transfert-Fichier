/**
 * @file reseau.h
 * @author Lilian CHARDON, Andréas Castello (lilian.chardon@ynov.com, andreas.castello@ynov.com)
 * @brief 
 * @version 0.1
 * @date 19-10-2023
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef RESEAU_H
#define RESEAU_H

#include "global.h"
#include "baseFonction.h"

/**
 * @brief Configure la structure d'addresse de socket pour des ports au hasard entre 1000 et 2000 et écouter sur toutes les
 * interfaces
 * 
 * @param addr l'addresse à configurer
 * 
 * @return 0 si succès, -1 sinon
 */
int8_t configAddr(struct sockaddr_in *addr);

/**
 * @brief Configure la socket pour écouter sur l'addresse spécifiée
 * 
 * @param descripteurDeSocketServeur le descripteur de la socket à configurer
 * @param addr l'addresse à écouter
 * 
 * @return 0 si succès, -1 sinon
 */
int8_t configSocket(int *descripteurDeSocketServeur, struct sockaddr_in *addr);

/**
 * @brief Vérifie si le port spécifié est déjà utilisé
 * 
 * @param port le port à vérifier
 * 
 * @return 1 si le port est utilisé, 0 sinon
 */
uint8_t portUsed(int port);


#endif // RESEAU_H