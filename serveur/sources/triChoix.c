/**
 * @file triChoix.c
 * @author Lilian CHARDON, Andréas Castello (lilian.chardon@ynov.com, andreas.castello@ynov.com)
 * @brief 
 * @version 0.1
 * @date 27-10-2023
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "triChoix.h"
#include "baseFonction.h"

void executeLsCommand(Client *client)
{
    system(requeteLs);
    FILE* fichier = fopen("resultat.txt", "r");
    char buffer[LONGUEUR_BUFFER];
    memset(buffer, 0, LONGUEUR_BUFFER);
    if (fichier)
    {
        fgets(buffer, LONGUEUR_BUFFER, fichier);
        printf("%s\n", buffer);
        send(
            client->descripteurDeSocketClient,
            buffer,
            LONGUEUR_BUFFER,
            0
        );
        fclose(fichier);
    }
    else
    {
        printfSyslog("Erreur d'ouverture du fichier", LOG_PERROR);
    }
    system("rm -f resultat.txt");
}

void changeDirectory(Client *client)
{
    char buffer[LONGUEUR_BUFFER];
    memset(buffer, 0, LONGUEUR_BUFFER);
    recv(
        client->descripteurDeSocketClient,
        buffer,
        LONGUEUR_BUFFER,
        0
    );
    
    char *cdDossier = malloc(sizeof(buffer) + 12);
    strcat(cdDossier, "partage/");
    strcat(cdDossier, buffer);

    printf("Dossier choisi : %s \n", buffer);
    syslog(LOG_INFO, "Dossier choisi : %s \n", buffer);
    chdir(cdDossier);
    free (cdDossier);
    send(
        client->descripteurDeSocketClient,
        "Repertoire changé.",
        LONGUEUR_BUFFER,
        0
    );
}

void downloadFile(Client *client)
{
    char buffer[LONGUEUR_BUFFER];
    memset(buffer, 0, LONGUEUR_BUFFER);
    recv(
        client->descripteurDeSocketClient,
        buffer,
        LONGUEUR_BUFFER,
        0
    );
    char dlFichier[LONGUEUR_BUFFER];
    char nomFichier[LONGUEUR_BUFFER];
    memset(dlFichier, 0, LONGUEUR_BUFFER);
    memset(nomFichier, 0, LONGUEUR_BUFFER);
    strcat(dlFichier, buffer);
    printf ("Fichier choisi : %s \n", dlFichier);
    syslog(LOG_INFO, "Fichier choisi : %s \n", dlFichier);
    chdir("partage");
    DIR* dir = opendir(buffer);
    if (dir)
    {
        closedir(dir);
        chdir(buffer);
        send(
            client->descripteurDeSocketClient,
            "Le dossier existe bien.",
            LONGUEUR_BUFFER, 
            0
        );

        system ("ls -C > resultat.txt");
        FILE* fichierLs = fopen("resultat.txt", "r");
        if (fichierLs)
        {
            // lecture du fichier pour obtenir le dernier chiffre présent dans le fichier
            int dernierChiffre = 0;
            char* version;
            char bufferLs[LONGUEUR_BUFFER];
            memset(bufferLs, 0, LONGUEUR_BUFFER);
            fgets(bufferLs, LONGUEUR_BUFFER, fichierLs);
            printf ("%s\n", bufferLs);
            fclose(fichierLs);
            system("rm -f resultat.txt");
            int i = 0;
            while (i < LONGUEUR_BUFFER)
            {
                if (bufferLs[i] == '.')
                {
                    dernierChiffre++;
                }
                i++;
            }
            dernierChiffre--;
            printf("Dernier chiffre : %d\n", dernierChiffre);
            version = itoa (dernierChiffre, 10);

            strcat(nomFichier, "-v");
            strcat(nomFichier, version);
            strcat(nomFichier, ".txt");

            strcat(dlFichier, nomFichier);

            printf("Fichier à envoyer : %s\n", dlFichier);
            syslog(LOG_INFO, "Fichier à envoyer : %s\n", dlFichier);
            
            FILE* fichierDl = fopen(dlFichier, "r");
            if (fichierDl)
            {
                send(
                    client->descripteurDeSocketClient,
                    "Le fichier existe bien, envoi en cours...",
                    LONGUEUR_BUFFER,
                    0
                );
                // on envoie le nom du fichier en 1er
                send(
                    client->descripteurDeSocketClient,
                    dlFichier,
                    LONGUEUR_BUFFER,
                    0
                );
                // on récupère le contenu du fichier
                fgets(buffer, LONGUEUR_BUFFER, fichierDl);
                // on envoie le contenu du fichier
                send(
                    client->descripteurDeSocketClient,
                    buffer,
                    LONGUEUR_BUFFER,
                    0
                );
                // on ferme le fichier
                fclose(fichierDl);
                chdir("..");
                chdir("..");
            }
            else
            {
                printfSyslog("Erreur d'ouverture du fichier", LOG_PERROR);
                send(
                    client->descripteurDeSocketClient,
                    "Erreur d'ouverture du fichier",
                    LONGUEUR_BUFFER,
                    0
                );
            }
        }
        else
        {
            printfSyslog("Erreur d'ouverture du fichier", LOG_PERROR);
        }
        system("rm -f resultat.txt");
    }
    else
    {
        send(
            client->descripteurDeSocketClient,
            "Fichier introuvable.",
            LONGUEUR_BUFFER,
            0
        );
    }
}

void receiveFile(Client *client)
{
    char buffer[LONGUEUR_BUFFER];
    memset(buffer, 0, LONGUEUR_BUFFER);
    recv(
        client->descripteurDeSocketClient,
        buffer,
        LONGUEUR_BUFFER,
        0
    );
    chdir("partage");
    char* nomDuFichier = buffer;
    DIR* dir = opendir(nomDuFichier);
    if (dir)
    {
        closedir(dir);
        printfSyslog("Le dossier existe déjà.\n", LOG_INFO);
        chdir(nomDuFichier);
        system ("ls -C > resultat.txt");
        FILE* fichierLs = fopen("resultat.txt", "r");
        if (fichierLs)
        {
            // lecture du fichier pour obtenir le dernier chiffre présent dans le fichier
            int dernierChiffre = 0;
            char* version;
            char bufferLs[LONGUEUR_BUFFER];
            fgets(bufferLs, LONGUEUR_BUFFER, fichierLs);
            fclose(fichierLs);
            system("rm -f resultat.txt");
            int i = 0;
            while (i < LONGUEUR_BUFFER)
            {
                if (bufferLs[i] == '.')
                {
                    dernierChiffre++;
                }
                i++;
            }
            printf("Dernier chiffre : %d\n", dernierChiffre);
            version = itoa (dernierChiffre, 10);

            strcat(nomDuFichier, "-v");
            strcat(nomDuFichier, version);   
        }
        else
        {
            printfSyslog("Erreur d'ouverture du fichier", LOG_PERROR);
        }
    }
    else
    {
        printfSyslog("Le dossier n'existe pas. Création en cours...\n", LOG_INFO);
        mkdir(nomDuFichier, 0777);
        printfSyslog("Dossier créé.\n", LOG_INFO);
        chdir(nomDuFichier);

        strcat(nomDuFichier, "-v1");
    }
    
    strcat(nomDuFichier, ".txt");
    FILE* fichier = fopen(nomDuFichier, "w");
    if (fichier)
    {
        recv(
            client->descripteurDeSocketClient,
            buffer,
            LONGUEUR_BUFFER,
            0
        );
        printfSyslog("Reception reussie.\n", LOG_INFO);
        send(
            client->descripteurDeSocketClient,
            "Reception reussie",
            LONGUEUR_BUFFER,
            0
        );
        fputs (buffer, fichier);
        fclose (fichier);
    }
    else
    {
        printfSyslog("Erreur d'ouverture du fichier", LOG_PERROR);
        send(
            client->descripteurDeSocketClient,
            "Erreur d'ouverture du fichier.",
            LONGUEUR_BUFFER,
            0
        );
    }
    chdir("..");
    chdir("..");
}

void exitClient(Client* client, uint8_t* sortieClient, uint8_t indexClient, uint8_t nbClient)
{
    // Fermeture de la connexion avec le client.
    if (close(client[indexClient].descripteurDeSocketClient) < 0)
    {
        printfSyslog("Problème avec la fermeture de la connexion avec le client", LOG_PERROR);
    }
    else
    {
        /* 
         * Définition du client à 0 dans le tableau, puis calcul de la taille du tableau client
         * moins la taille de la structure client pour ensuite vérifier si la valeur du résultat nvaleur
         * est inférieure ou égale à la valeur maximale d'un pointeur. Si c'est vrai, alors on réalloue
         * la mémoire du tableau client à la valeur calculer.
         */
        client[indexClient] = (Client){0};
        for (uint8_t i = indexClient; i + 1 < nbClient; i++)
            client[i] = client[indexClient + 1];
        
        printfSyslog("Le client s'est bien deconnecté.\n", LOG_INFO);
    }
}

void shutdownServer(uint8_t *sortie, uint8_t *sortieClient, uint8_t nbClient, int *descripteurDeSocketServeur, Client *client)
{
    *sortie = 0;
    *sortieClient = 0;
    
    /* Ferme le socket de chaque client. */
    for (int i = 0; i < nbClient; i++)
    {
        if(shutdown(descripteurDeSocketServeur[i], SHUT_RD) < 0)
        {
            printfSyslog("Problème avec le shutdown.", LOG_PERROR);
            send(
                client->descripteurDeSocketClient,
                "Problème avec le shutdown.",
                LONGUEUR_BUFFER,
                0
            );
        }
        else if (i == nbClient - 1)
        {
            printfSyslog("Shutdown effectué.\n", LOG_INFO);
            send(
                client->descripteurDeSocketClient,
                "Shutdown effectué.",
                LONGUEUR_BUFFER,
                0
            );
        }

        close(client[i].descripteurDeSocketClient);
        client[i] = (Client){0};
    }

    nbClient = 0;
}
