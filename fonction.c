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
 * @brief fonction str_eq, test légalité de 2 chaînes de caractères.
 * 
 * @param str1 La première chaîne à comparer.
 * @param str2 La chaîne à comparer.
 * 
 * @return 1 si les deux chaînes sont égales, 0 sinon.
 */
int str_eq(const char *str1, const char *str2)
{
  while (*str1 == *str2 && *str1)
  {
    str1++;
    str2++;
  }
  return *str1 == *str2;
}

/**
 * @brief fonction config_addr, configuration adresse de la socket et la remplit avec le port et l'adresse du serveur.
 * 
 * @param addr : adresse de la socket
 * 
 * @return 0 si succès, -1 sinon
 */
int8_t config_addr(struct sockaddr_in *addr)
{
    addr->sin_family = PF_INET;
    addr->sin_port = htons(2000);
    addr->sin_addr.s_addr = INADDR_ANY;
    return 0;
}

/**
 * @brief fonction stockage_client, stockage du client dans la structure Client.
 * 
 * @param client : structure client
 * @param longueurDeAdresseDuClient : longueur de l'adresse du client
 */
void stockage_client(Client *client, int descripteurDeSocketServeur)
{
    /* On obtient le nombre d'éléments dans le tableau. */
    uint8_t taille = 1;
    uint8_t indexClient;

    /* Boucle infinie qui accepte un client et l'ajoute au tableau client. */
    while (1)
    {
        indexClient = taille - 1;
        /* Accepte un client et l'ajoute au tableau client. */
        client[indexClient].descripteurDeSocketClient = accept(
            descripteurDeSocketServeur,
            (struct sockaddr *)&(client[indexClient].adresseDuClient),
            &(client[indexClient].longueurDeAdresseDuClient)
        );
        taille++;

        /* Vérifie si le client est connecté. */
        if (client[indexClient].descripteurDeSocketClient < 0)
        {
            perror("Erreur de connexion.\n");
            free(client);
            client = NULL;
            return;
        }
        else if (taille < 4)
        {
            printf("Client connecté.\n");

            /* On prévient le client qu'il est bien connecté et réalloue la mémoire pour laisser
            place à un nouveau client. */
            send (client[indexClient].descripteurDeSocketClient, "Bienvenue sur le serveur.\n", 30, 0);
            client = (Client*) realloc(client, sizeof(client) + sizeof(Client));
            if (!client)
            {
                perror("Erreur d'allocation de mémoire.\n");
                free(client);
                client = NULL;
                return;
            }
        }
        else
        {
            printf ("Le nombre de clients maximum est atteint.\n");
            send (client[indexClient].descripteurDeSocketClient, "Le nombre de clients maximum est atteint.\n", 50, 0);
            close (client[indexClient].descripteurDeSocketClient);
            taille--;
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
int8_t reception_client(Client *client, uint8_t* taille, char** buffer)
{
    /* Initialisation de l'index du tableau de buffer à 0. */
    uint8_t i = 0;
    /* Variable utilisée pour stocker le nombre d'octets lus. */
    int nb_octets_lus;

    /* Boucle infinie qui reçoit le message du client. */
    while (1)
    {
        nb_octets_lus = 0;
        if (i != taille)
        {
            /* Reçoit le message du client. */
            if (recv(
                client[i].descripteurDeSocketClient,
                buffer[i],
                LONGUEUR_BUFFER,
                0
            ))
            {
                /* Vérifie si le message n'est pas vide. */
                if (buffer[i][0] != '\0')
                {
                    printf("\nLecture de la requete : \n");
                    /* On lit le message du client. */
                    do
                    {
                        nb_octets_lus = recv(
                            client[i].descripteurDeSocketClient,
                            buffer[i],
                            LONGUEUR_BUFFER,
                            0
                        );
                    }
                    while (nb_octets_lus < LONGUEUR_BUFFER);
                    printf("%s\n", buffer[i]);
                    printf("Lecture de la requete terminée.\n\n");
                    send(client[i].descripteurDeSocketClient, "Requete lue.\n", 20, 0);
                }

            i++;
            }
        }
        else /*--->*/ i = 0;
    }
}