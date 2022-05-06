/**
 * @file fonction.c
 * @author Lilian CHARDON (lilian.chardon@ynov.com)
 * @brief 
 * @version 0.1
 * @date 05-05-2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include "fonction.h"

/**
 * @brief fonction config_addr, configuration adresse de la socket et la remplit avec le port et l'adresse du serveur.
 * 
 * @param sockaddr_in* addr : adresse de la socket
 * 
 * @return int : 0 si succès, -1 sinon
 */
int config_addr(struct sockaddr_in *addr)
{
    addr->sin_family = PF_INET;
    addr->sin_port = htons(2000);
    addr->sin_addr.s_addr = INADDR_ANY;
    return 0;
}

/**
 * @brief fonction stockage_client, stockage du client dans la structure Client.
 * 
 * @param Client* client : structure client
 * @param unsigned int longueurDeAdresseDuClient : longueur de l'adresse du client
 */
void stockage_client(Client *client, int descripteurDeSocketServeur)
{
    /* On obtient le nombre d'éléments dans le tableau. */
    uint8_t taille;

    /* Boucle infinie qui accepte un client et l'ajoute au tableau client. */
    while (1)
    {
        taille = sizeof(client) / sizeof(Client) - 1;
        /* Accepte un client et l'ajoute au tableau client. */
        client[taille].descripteurDeSocketClient = accept(
            descripteurDeSocketServeur,
            (struct sockaddr *)&(client[taille].adresseDuClient),
            &(client[taille].longueurDeAdresseDuClient)
        );

        /* Vérifie si le client est connecté. */
        if (client[taille].descripteurDeSocketClient < 0)
        {
            perror("Erreur de connexion.\n");
            free(client);
            client = NULL;
            return;
        }
        else
        {
            printf("Client connecté.\n");
            
            client = (Client*) realloc(client, sizeof(client) + sizeof(Client));
        }
    }
}

/**
 * @brief fonction reception_client, reception du message du client.
 * 
 * @param client : structure client
 * @param taille : taille du tableau
 * @param buffer : message du client
 * 
 * @return int : 0 si succès, -1 sinon
 */
int reception_client(Client *client, uint8_t taille, char* buffer)
{
    uint8_t i = 0;
    /* Boucle infinie qui reçoit le message du client. */
    while (1)
    {
        if (i != taille)
        {
            /* Reçoit le message du client. */
            recv(
                client[i].descripteurDeSocketClient,
                buffer[LONGUEUR_BUFFER * i],
                LONGUEUR_BUFFER,
                0
            );
            i++;
        }
        else /*--->*/ i = 0;
    }
}