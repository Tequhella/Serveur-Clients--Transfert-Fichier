/**
 * @file gestionClient.c
 * @author Lilian CHARDON, Andréas Castello (lilian.chardon@ynov.com, andreas.castello@ynov.com)
 * @brief 
 * @version 0.1
 * @date 19-10-2023
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "gestionClient.h"
#include "baseFonction.h"
#include "reseau.h"
#include "triChoix.h"

/**
 * @brief fonction stockageClient, stockage du client dans la structure Client.
 * 
 * @param argumentThread : structure contenant le client et le descripteur de socket du serveur.
 * (client : structure client)
 */
void* stockageClient(void* argumentThread)
{
    Client client[NB_CLIENT_MAX]                         = {0};
    int*   descripteurDeSocketServeur                    = ((ArgumentThreadClient*)argumentThread)->descripteurDeSocketServeur;
    uint8_t nbClient                                     = 0;
    uint8_t indexClient                                  = 0;
    pthread_t threadReception[NB_CLIENT_MAX]             = {0};
    uint8_t stopAccept4                                  = 0;
    struct sockaddr_in* adresseDuServeur                 = ((ArgumentThreadClient*)argumentThread)->adresseDuServeur;
    uint8_t sortie                                       = 1;

    while (sortie == 1)
    {
        client[indexClient].longueurDeAdresseDuClient = sizeof(struct sockaddr_in);
        while (stopAccept4 == 0 && nbClient < NB_CLIENT_MAX && sortie == 1)
        {
            /* Accepte un client et l'ajoute au tableau client. */
            client[indexClient].descripteurDeSocketClient = accept4(
                descripteurDeSocketServeur[indexClient],
                (struct sockaddr*)&(client[indexClient].adresseDuClient),
                &(client[indexClient].longueurDeAdresseDuClient),
                SOCK_NONBLOCK
            );

            /**
             * This code block checks if the client's socket descriptor is less than 0. 
             * If it is, it checks if the error is EAGAIN or EWOULDBLOCK. 
             * If it is, it prints an error message to the syslog and standard output, and sets stopAccept4 to 1. 
             * If the error is not EAGAIN or EWOULDBLOCK, it frees the client array, joins the reception threads, 
             * frees the threadReception array, and returns NULL. 
             * If the client's socket descriptor is greater than 0, it sets stopAccept4 to 1.
             *
             * @param[in] indexClient The index of the client.
             * @param[in] descripteurDeSocketServeur The server's socket descriptor.
             * @param[in] client The array of clients.
             * @param[in] taille The size of the client array.
             * @param[in] threadReception The array of reception threads.
             * @param[in] stopAccept4 The flag to stop accepting clients.
             * @return NULL if an error occurs, otherwise nothing.
             */
            if (client[indexClient].descripteurDeSocketClient < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    /**
                     * This code block prints an error message to the system log and explains the error that occurred during the acceptance of a client connection.
                     * It also sets the stopAccept4 flag to 1.
                     * 
                     * @param errno The error number that occurred during the acceptance of the client connection.
                     * @param descripteurDeSocketServeur The server socket descriptor.
                     * @param client The client socket address structure.
                     * @param longueurDeAdresseDuClient The length of the client socket address structure.
                     * @param SOCK_NONBLOCK The socket flag to set the socket to non-blocking mode.
                     */
                    printfSyslog("Erreur d'acceptation du client", LOG_PERROR);

                    printf("%s", explain_errno_accept4(
                            errno,
                            descripteurDeSocketServeur[indexClient],
                            (struct sockaddr*)&(client[indexClient].adresseDuClient),
                            &(client[indexClient].longueurDeAdresseDuClient),
                            SOCK_NONBLOCK
                        )
                    );
                    stopAccept4 = 1;
                }
                else
                {
                    printfSyslog("Erreur d'acceptation du client", LOG_PERROR);

                    for (uint8_t i = 0; i < nbClient; i++)
                    {
                        verifJoinThread(threadReception, i);
                        threadReception[i] = 0;
                        close(client[i].descripteurDeSocketClient);
                        client[i] = (Client){0};                        
                    }

                    return NULL;
                }
            }
            else if (client[indexClient].descripteurDeSocketClient > 0)
            {
                stopAccept4 = 1;
            }
        }
        
        /* Vérifie si le nombre de clients est inférieur à 4. */
        if (nbClient < 4)
        {
            printfSyslog("Client connecté.\n", LOG_INFO);

            /* On récupère l'adresse IP du client et l'imprime avec le descripteur de socket la longueur de l'adresse */
            unsigned char* adresse = (unsigned char*)&client[indexClient].adresseDuClient.sin_addr.s_addr;
            printf(
                "Contenu du client %d : %d.%d.%d.%d, %d, %d\n",
                indexClient,
                *(adresse),
                *(adresse + 1),
                *(adresse + 2),
                *(adresse + 3),
                client[indexClient].descripteurDeSocketClient,
                client[indexClient].longueurDeAdresseDuClient
            );
            syslog(
                LOG_INFO,
                "Contenu du client %d : %d.%d.%d.%d, %d, %d\n",
                indexClient,
                *(adresse),
                *(adresse + 1),
                *(adresse + 2),
                *(adresse + 3),
                client[indexClient].descripteurDeSocketClient,
                client[indexClient].longueurDeAdresseDuClient
            );


            /* On prévient le client qu'il est bien connecté et on réalloue la mémoire pour laisser
            place à un nouveau client. */
            printfSyslog("Envoi de la réponse au client.\n", LOG_INFO);
            send (client[indexClient].descripteurDeSocketClient, "Bienvenue sur le serveur.\n", 30, 0);

            stopAccept4 = 0;
            nbClient++;
            
            printfSyslog("Client ajouté.\n", LOG_INFO);

            ArgumentThreadClient argumentThreadClient = {
                .client = client,
                .descripteurDeSocketServeur = descripteurDeSocketServeur,
                .indexClient = indexClient,
                .nbClient = nbClient,
                .sortie = &sortie,
                .sortieClient = NULL,
                .adresseDuServeur = adresseDuServeur
            };

            /* Création du thread de réception. */
            if (pthread_create(&threadReception[indexClient], NULL, receptionClient, (void*)&argumentThreadClient) != 0)
            {
                printfSyslog("Erreur de création du thread de réception", LOG_PERROR);
                
                for (uint8_t i = 0; i < nbClient; i++)
                {
                    verifJoinThread(threadReception, i);
                    threadReception[i] = 0;
                    close(client[i].descripteurDeSocketClient);
                    client[i] = (Client){0};                    
                }
                
                return NULL;
            }
            else
            {
                printfSyslog("Thread de réception créé", LOG_INFO);

                if (indexClient == NB_CLIENT_MAX - 1) /*--->*/ indexClient = NB_CLIENT_MAX - 1;
                else /*------------------------------------>*/ indexClient++;
                
                /**
                 * Configures the server address and socket.
                 *
                 * @param adresseDuServeur The server address to configure.
                 * @param descripteurDeSocketServeur The server socket descriptor to configure.
                 */
                configAddr(adresseDuServeur);
                configSocket(descripteurDeSocketServeur, adresseDuServeur);
            }
        }
        else
        {
            printfSyslog("Le nombre de clients maximum est atteint.\n", LOG_INFO);
            
            send (client[indexClient].descripteurDeSocketClient, "Le nombre de clients maximum est atteint.\n", 50, 0);
            close (client[indexClient].descripteurDeSocketClient);
        }
    }

    for (uint8_t i = 0; i < nbClient; i++)
    {
        verifJoinThread(threadReception, i);
        threadReception[i] = 0;
        close(client[i].descripteurDeSocketClient);
        client[i] = (Client){0};        
    }

    return NULL;
}


/**
 * @brief Reçoit la requête du client et l'envoie à la fonction qui la traitera
 * 
 * @param argumentThread structure contenant le client, l'index du client, la taille et du stock de client et le buffer.
 * (client : structure client)
 * (indexClient : index du client)
 * (taille : taille du tableau)
 * (sortie : sortie du serveur)
 */
void* receptionClient(void* argumentThread)
{
    Client*  client       = ((ArgumentThreadClient*)argumentThread)->client;
    uint8_t  indexClient  = ((ArgumentThreadClient*)argumentThread)->indexClient;
    uint8_t* sortie       = ((ArgumentThreadClient*)argumentThread)->sortie;
    uint8_t  sortieClient = 1;

    /* On récupère le buffer. */
    char buffer[LONGUEUR_BUFFER];

    while (sortieClient == 1 && *sortie == 1)
    {
        memset(buffer, 0, LONGUEUR_BUFFER);

        /* Lecture du message du client. */
        printfSyslog("\nLecture de la requete : ", LOG_INFO);
        while (buffer == NULL || buffer[0] == '\0')
        {
            recv(
                client[indexClient].descripteurDeSocketClient,
                buffer,
                LONGUEUR_BUFFER,
                0
            );
        }
        printf("%s\n", buffer);
        syslog(LOG_INFO, "%s\n", buffer);

        printfSyslog("Lecture de la requete terminée.\n\n", LOG_INFO);

        /* On envoie la confirmation au client. */
        send(client[indexClient].descripteurDeSocketClient, "Requete lue.", 20, 0);

        // choix du client
        if (strEq(buffer, requeteLs))
        {
            executeLsCommand(&client[indexClient]);
        }
        else if (strEq(buffer, requeteCd))
        {
            changeDirectory(&client[indexClient]);
        }
        else if (strEq(buffer, requeteGet))
        {
            downloadFile(&client[indexClient]);
        }
        else if (strEq(buffer, requetePut))
        {
            receiveFile(&client[indexClient]);
        }
        else if (strEq(buffer, requeteExit))
        {
            exitClient(client, &sortieClient, indexClient, ((ArgumentThreadClient*)argumentThread)->nbClient);
        }
        else if (strEq(buffer, requeteShutdown))
        {
            shutdownServer(sortie, &sortieClient, ((ArgumentThreadClient*)argumentThread)->nbClient, ((ArgumentThreadClient*)argumentThread)->descripteurDeSocketServeur, client);
        }
        else
        {
            printfSyslog("Commande inconnue.\n", LOG_INFO);
            send(client[indexClient].descripteurDeSocketClient, "Commande inconnue.", 20, 0);
        }
    }
    return NULL;
}