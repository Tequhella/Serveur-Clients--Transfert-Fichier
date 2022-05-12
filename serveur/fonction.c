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
 * @brief fonction itoa, convertit un entier en une chaîne de caractères.
 * 
 * @param val : entier à convertir.
 * @param base : base de la conversion.
 * @return char* : chaîne de caractères correspondant à l'entier.
 */
char* itoa(int val, int base)
{
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
}
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
void stockage_client(Client* client, int descripteurDeSocketServeur)
{
    /* On obtient le nombre d'éléments dans le tableau. */
    uint8_t taille = 1;
    uint8_t indexClient;

    indexClient = taille - 1;
        /* Accepte un client et l'ajoute au tableau client. */
        client->descripteurDeSocketClient = accept(
            descripteurDeSocketServeur,
            (struct sockaddr *)&(client->adresseDuClient),
            &(client->longueurDeAdresseDuClient)
        );
        taille++;

        /* Vérifie si le client est connecté. */
        if (client->descripteurDeSocketClient < 0)
        {
            perror("Erreur de connexion.\n");
            syslog(LOG_ERR, "Erreur de connexion.\n");
            free(client);
            client = NULL;
            return;
        }
        else if (taille < 4)
        {
            printf("Client connecté.\n");
            syslog(LOG_INFO, "Client connecté.\n");

            /* On prévient le client qu'il est bien connecté et réalloue la mémoire pour laisser
            place à un nouveau client. */
            printf ("Envoi de la réponse au client.");
            syslog(LOG_INFO, "Envoi de la réponse au client.");
            send (client->descripteurDeSocketClient, "Bienvenue sur le serveur.\n", 30, 0);
            /*
            client = (Client*) realloc(client, sizeof(client) + sizeof(Client));
            if (!client)
            {
                perror("Erreur d'allocation de mémoire.\n");
                free(client);
                client = NULL;
                return;
            }
            */
        }
        else
        {
            printf ("Le nombre de clients maximum est atteint.\n");
            syslog(LOG_INFO, "Le nombre de clients maximum est atteint.\n");
            send (client->descripteurDeSocketClient, "Le nombre de clients maximum est atteint.\n", 50, 0);
            close (client->descripteurDeSocketClient);
            taille--;
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
int8_t reception_client(Client *client, uint8_t* taille, char* buffer)
{
    /* Variable utilisée pour stocker le nombre d'octets lus. */
    int nb_octets_lus;
    printf("\nLecture de la requete : \n");
    syslog(LOG_INFO, "Lecture de la requete : \n");
    recv(
        client->descripteurDeSocketClient,
        buffer,
        LONGUEUR_BUFFER,
        0
    );
    printf("%s\n", buffer);
    syslog(LOG_INFO, "%s\n", buffer);
    printf("Lecture de la requete terminée.\n\n");
    syslog(LOG_INFO, "Lecture de la requete terminée.\n\n");
    send(client->descripteurDeSocketClient, "Requete lue.", 20, 0);
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
            printf("Erreur d'ouverture du fichier.\n");
            syslog(LOG_ERR, "Erreur d'ouverture du fichier.\n");
        }
        system("rm -f resultat.txt");
    }
    else if (str_eq(choix, requeteCd))
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

        printf("Vous avez choisis le dossier : %s \n", buffer);
        syslog(LOG_INFO, "Vous avez choisis le dossier : %s \n", buffer);
        chdir(cdDossier);
        free (cdDossier);
        send(
            client->descripteurDeSocketClient,
            "Repertoire changé.",
            LONGUEUR_BUFFER,
            0
        );
    }
    else if (str_eq(choix, requeteDl))
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
        printf ("Vous avez choisis le fichier : %s \n", dlFichier);
        syslog(LOG_INFO, "Vous avez choisis le fichier : %s \n", dlFichier);
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
                dernierChiffre--;
                printf("Dernier chiffre : %d\n", dernierChiffre);
                version = itoa (dernierChiffre, 10);

                strcat(nomFichier, "-v");
                strcat(nomFichier, version);
                strcat(nomFichier, ".txt");

                strcat(dlFichier, nomFichier);

                printf ("Fichier à envoyer : %s\n", dlFichier);
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
                    printf("Erreur d'ouverture du fichier.\n");
                    syslog(LOG_ERR, "Erreur d'ouverture du fichier.\n");
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
                printf("Erreur d'ouverture du fichier.\n");
                syslog(LOG_ERR, "Erreur d'ouverture du fichier.\n");
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
    else if (str_eq(choix, requeteSend))
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
            printf("Le dossier existe déjà.\n");
            syslog(LOG_INFO, "Le dossier existe déjà.\n");
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
                printf("Erreur d'ouverture du fichier.\n");
                syslog(LOG_ERR, "Erreur d'ouverture du fichier.\n");
            }
        }
        else
        {
            printf("Le dossier n'existe pas. Création en cours...\n");
            syslog(LOG_INFO, "Le dossier n'existe pas. Création en cours...\n");
            mkdir(nomDuFichier, 0777);
            printf("Dossier créé.\n");
            syslog(LOG_INFO, "Dossier créé.\n");
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
            printf("Reception reussie.\n");
            syslog(LOG_INFO, "Reception reussie.\n");
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
            printf ("Erreur d'ouverture du fichier.");
            syslog(LOG_ERR, "Erreur d'ouverture du fichier.");
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
    else if (str_eq(choix, requeteExit))
    {
        *sortie = 0;
        /*
        close(client->descripteurDeSocketClient);
        
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
        */
    }
    else if (str_eq(choix, requeteShutdown))
    {
        *sortie = 0;
        send(
            client->descripteurDeSocketClient,
            "Shutdown effectué.",
            LONGUEUR_BUFFER,
            0
        );
    }
}