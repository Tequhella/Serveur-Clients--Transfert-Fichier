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
uint8_t str_eq(const char *str1, const char *str2)
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
int8_t config_addr(struct sockaddr_in* addr)
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
                    send(client[i].descripteurDeSocketClient, "Requete lue.", 20, 0);
                }

            i++;
            }
        }
        else /*--->*/ i = 0;
    }
}

/**
 * @brief fonction tri_choix, tri du choix du client.
 * 
 * @param client : structure client
 * @param indexClient : index du client
 * @param choix : choix du client
 * @param sortie : sortie du programme
 */
void tri_choix(Client* client, uint8_t indexClient, char* choix, uint8_t* sortie)
{
    /* On trie le choix du client. */
    if (str_eq(choix, requeteLs))
    {
        system(requeteLs);
        FILE* fichier = fopen("resultat.txt", "r");
        char buffer[LONGUEUR_BUFFER];
        if (fichier)
        {
            fgets(buffer, LONGUEUR_BUFFER, fichier);
            send(client[indexClient].descripteurDeSocketClient, buffer, LONGUEUR_BUFFER, 0);
            fclose(fichier);
        }
        else
        {
            printf("Erreur d'ouverture du fichier.\n");
        }
        system("rm -f resultat.txt");
    }
    else if (str_eq(choix, requeteCd))
    {
        char buffer[LONGUEUR_BUFFER];
        int nb_octets_lus = 0;
        do
        {
            nb_octets_lus = recv(
                client[indexClient].descripteurDeSocketClient,
                buffer,
                LONGUEUR_BUFFER,
                0
            );
        }
        while (nb_octets_lus < LONGUEUR_BUFFER);
        
        char *cdDossier = malloc(sizeof(buffer) + 12);
        strcat(cdDossier, "partage/");
        strcat(cdDossier, buffer);

        printf("Vous avez choisis le dossier : %s \n", buffer);
        chdir(cdDossier);
        free (cdDossier);
        send(client[indexClient].descripteurDeSocketClient, "Repertoire changé.", 20, 0);
    }
    else if (str_eq(choix, requeteDl))
    {
        char buffer[LONGUEUR_BUFFER];
        int nb_octets_lus = 0;
        send(client[indexClient].descripteurDeSocketClient, "Téléchargement en cours.", 20, 0);
        do
        {
            nb_octets_lus = recv(
                client[indexClient].descripteurDeSocketClient,
                buffer,
                LONGUEUR_BUFFER,
                0
            );
        }
        while (nb_octets_lus < LONGUEUR_BUFFER);
        char *dlFichier = malloc(sizeof(buffer) + 12);
        strcat(dlFichier, "partage/");
        strcat(dlFichier, buffer);

        FILE* fichier = fopen(dlFichier, "r");
        if (fichier)
        {
            send(client[indexClient].descripteurDeSocketClient, "Fichier trouvé.", 20, 0);

            /* On récupère le nom du fichier. */
            char nomFichier[LONGUEUR_BUFFER];
            fgets(nomFichier, LONGUEUR_BUFFER, fichier);
            /* On récupère le contenu du fichier. */
            char contenuFichier[LONGUEUR_BUFFER];
            fgets(contenuFichier, LONGUEUR_BUFFER, fichier);
            /* On ferme le fichier. */
            fclose(fichier);
            /* On envoie le nom du fichier. */
            send(client[indexClient].descripteurDeSocketClient, nomFichier, LONGUEUR_BUFFER, 0);
            /* On envoie le contenu du fichier. */
            send(client[indexClient].descripteurDeSocketClient, contenuFichier, LONGUEUR_BUFFER, 0);
        }
        else
        {
            printf("Erreur d'ouverture du fichier.\n");
        }
    }
    else if (str_eq(choix, requeteSend))
    {
        char buffer[LONGUEUR_BUFFER];
        int nb_octets_lus = 0;
        send(client[indexClient].descripteurDeSocketClient, "Envoie en cours.", 20, 0);
        do
        {
            nb_octets_lus = recv(
                client[indexClient].descripteurDeSocketClient,
                buffer,
                LONGUEUR_BUFFER,
                0
            );
        }
        while (nb_octets_lus < LONGUEUR_BUFFER);
        
        char* nomDuFichier = buffer;
        FILE* fichier = fopen(nomDuFichier, "w");
        if (fichier)
        {
            do
            {
                nb_octets_lus = recv(
                    client[indexClient].descripteurDeSocketClient,
                    buffer,
                    LONGUEUR_BUFFER,
                    0
                );
            }
            while (nb_octets_lus < LONGUEUR_BUFFER);
            fputs (buffer, fichier);
        }
        else
        {
            printf ("Le fichier %s n'a pas pu être ouvert.\n", nomDuFichier);
        }
        fclose (fichier);

    }
    else if (str_eq(choix, requeteExit))
    {
        close(client[indexClient].descripteurDeSocketClient);
        if (indexClient == 2)
        {
            client = (Client*) realloc(client, sizeof(client) - sizeof(Client));
            if (!client)
            {
                perror("Erreur d'allocation de mémoire.\n");
                free(client);
                client = NULL;
                exit(-1);
            }
        }
        else
        {
            client[indexClient] = client[sizeof(client) / sizeof(Client) - 1];
            client = (Client*) realloc(client, sizeof(client) - sizeof(Client));
            if (!client)
            {
                perror("Erreur d'allocation de mémoire.\n");
                free(client);
                client = NULL;
                exit(-1);
            }
        }
    }
    else if (str_eq(choix, requeteShutdown))
    {
        sortie = 0;
        send(client->descripteurDeSocketClient, "Shutdown effectué.", 20, 0);
    }
}