/**
 * @file triChoix.h
 * @author Lilian CHARDON, Andréas Castello (lilian.chardon@ynov.com, andreas.castello@ynov.com)
 * @brief 
 * @version 0.1
 * @date 27-10-2023
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef TRI_CHOIX_H
#define TRI_CHOIX_H

#include "global.h"
#include "gestionClient.h"

/**
 * Executes the "ls" command for the given client.
 * 
 * @param client A pointer to the client object.
 */
void executeLsCommand(Client* client);

/**
 * Changes the current working directory of the specified client.
 * 
 * @param client A pointer to the client whose working directory will be changed.
 */
void changeDirectory(Client* client);

/**
 * Downloads a file from the server to the specified client.
 *
 * @param client A pointer to the client that will receive the file.
 */
void downloadFile(Client* client);

/**
 * Uploads a file from the specified client to the server.
 *
 * @param client A pointer to the client that will send the file.
 */
void receiveFile(Client* client);


/**
 * @brief Exits the client and sets the exit status and client exit status.
 * 
 * @param client Pointer to the client to exit.
 * @param sortieClient Pointer to the client exit status to set.
 * @param indexClient Index of the client to exit.
 * @param nbClient Number of clients connected to the server.
 */

void exitClient(Client* client, uint8_t* sortieClient, uint8_t indexClient, uint8_t nbClient);

/**
 * @brief Shutdown the server and sets the exit status and client exit status.
 * 
 * @param sortie Pointer to the server exit status to set.
 * @param sortieClient Pointer to the client exit status to set.
 * @param nbClient Number of clients connected to the server.
 * @param descripteurDeSocketServeur Pointer to the server socket descriptor.
 * @param client Pointer to the client to exit.
 */
void shutdownServer(uint8_t* sortie, uint8_t* sortieClient, uint8_t nbClient, int* descripteurDeSocketServeur, Client* client);

#endif