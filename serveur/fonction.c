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
 * @brief fonction strEq, test légalité de 2 chaînes de caractères.
 * 
 * @param str1 La première chaîne à comparer.
 * @param str2 La chaîne à comparer.
 * 
 * @return 1 si les deux chaînes sont égales, 0 sinon.
 */
uint8_t strEq(const char *str1, const char *str2)
{
  while (*str1 == *str2 && *str1)
  {
    str1++;
    str2++;
  }
  return *str1 == *str2;
}

/**
 * @brief fonction printfSyslog, imprime la chaîne donnée sur la console et via le système de journalisation.
 * 
 * @param str La chaîne à imprimer.
 */
void printfSyslog(const char* str, const uint8_t logType)
{
    if (logType == LOG_PERROR)
    {
        perror(str);
        syslog(logType, "%s", str);
    }
    else
    {
        printf("%s", str);
        syslog(logType, "%s", str);
    }
}

/**
 * @brief Configure la structure d'addresse de socket pour utiliser le port 2000 et écouter sur toutes les
 * interfaces
 * 
 * @param addr l'addresse à configurer
 * 
 * @return 0 si succès, -1 sinon
 */
int8_t configAddr(struct sockaddr_in* addr)
{
    addr->sin_family = PF_INET;
    addr->sin_port = htons(2000);
    addr->sin_addr.s_addr = INADDR_ANY;
    return 0;
}

/**
 * @brief fonction verifJoinThread, vérifie la fin de fonctionnement du thread.
 * 
 * @param thread Le thread donné à vérifier.
 * @param index Index du tableau de thread si c'en est un.
 * 
 */
void verifJoinThread(pthread_t* thread, uint8_t index)
{
    void* retval;
    int ret = pthread_join(thread[index], &retval);
    if (retval == PTHREAD_CANCELED)
        printfSyslog("Le thread a été annulé - ", LOG_INFO);
    else
    {
        printf("Valeur retournée %ld - ", (unsigned long)retval);
        syslog(LOG_INFO, "Valeur retournée %ld -", (unsigned long)retval);
    }

    switch (ret)
    {
        case 0:
            printfSyslog("Le thread a été joigné avec succès\n", LOG_INFO);
            break;
        case EDEADLK:
            printfSyslog("Impasse detectée\n", LOG_ERR);
            break;
        case EINVAL:
            printfSyslog("Le thread n'est pas joignable\n", LOG_ERR);
            break;
        case ESRCH:
            printfSyslog("Aucun thread avec l'ID donnée n'a été trouvée\n", LOG_ERR);
            break;
        default:
            printfSyslog("Une erreur est survenue lors du joignage\n", LOG_ERR);
    }
}

/**
 * @brief fonction stockageClient, stockage du client dans la structure Client.
 * 
 * @param argumentThread : structure contenant le client et le descripteur de socket du serveur.
 * (client : structure client)
 */
void* stockageClient(void* argumentThread)
{
    unsigned long tailleMemoire       = sizeof(Client);
    unsigned long tailleMemoireThread = sizeof(pthread_t);
    Client* client = (Client*) malloc (tailleMemoire); // client créé et sera agrandit à chaque fois qu'un client se connecte
    if (!client)
    {
        printfSyslog("Erreur d'allocation de mémoire du client", LOG_PERROR);
        return NULL;
    }
    client->descripteurDeSocketClient = 0;
    int    descripteurDeSocketServeur = ((ArgumentThreadClient*)argumentThread)->descripteurDeSocketServeur;
    uint8_t taille                    = 1;
    uint8_t indexClient               = 0;
    pthread_t* threadReception        = (pthread_t*) malloc(tailleMemoireThread); /* Thread de réception. */
    if (!threadReception)
    {
        printfSyslog("Erreur d'allocation de mémoire du thread de reception", LOG_PERROR);
        return NULL;
    }

    while (*((ArgumentThreadClient*)argumentThread)->sortie == 1)
    {
        /* On obtient la taille de l'adresse du client. */
        client[indexClient].longueurDeAdresseDuClient = sizeof(struct sockaddr_in);
        /* Accepte un client et l'ajoute au tableau client. */
        client[indexClient].descripteurDeSocketClient = accept(
            descripteurDeSocketServeur,
            (struct sockaddr*)&(client[indexClient].adresseDuClient),
            &(client[indexClient].longueurDeAdresseDuClient)
        );

        /* Calcul de la taille du tableau client. */
        taille = tailleMemoire / sizeof(Client);

        /* Vérifie si le client est connecté. */
        if (client[indexClient].descripteurDeSocketClient < 0)
        {
            printfSyslog("Erreur de connexion", LOG_PERROR);

            if(client)
            {
                for (uint8_t i = 0; i < taille - 1; i++)
                    verifJoinThread(threadReception, i);

                free(client);
                client = NULL;
                free(threadReception);
                threadReception = NULL;
            }
            return NULL;
        }
        else if (taille < 4)
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
            
            /* Reallocation de mémoire pour ajouter un futur client. */
            tailleMemoire += sizeof(Client);
            Client* client_temp = (Client*) realloc(client, tailleMemoire);
            
            /* Vérifie si le client n'est pas nul. */
            if (!client_temp)
            {
                printfSyslog("Erreur d'allocation de mémoire du client", LOG_PERROR);
                
                if (client)
                {
                    free(client);
                    client = NULL;
                }
                return NULL;
            }
            else
            {
                printfSyslog("Client ajouté.\n", LOG_INFO);

                client = client_temp;
                client_temp = NULL;

                ArgumentThreadClient argumentThreadClient = {
                    .client = client,
                    .descripteurDeSocketServeur = descripteurDeSocketServeur,
                    .indexClient = indexClient,
                    .taille = tailleMemoire,
                    .sortie = ((ArgumentThreadClient*)argumentThread)->sortie,
                    .sortieClient = NULL,
                };

                /* Création du thread de réception. */
                if (pthread_create(&threadReception[indexClient], NULL, receptionClient, (void*)&argumentThreadClient) != 0)
                {
                    printfSyslog("Erreur de création du thread de réception", LOG_PERROR);
                    
                    free(client);
                    client = NULL;
                    return NULL;
                }
                else
                {
                    printfSyslog("Thread de réception créé", LOG_INFO);

                    tailleMemoireThread += sizeof(pthread_t);

                    pthread_t* threadReceptionTemp = (pthread_t*) realloc(threadReception, tailleMemoireThread);
                    if (!threadReceptionTemp)
                    {
                        printfSyslog("Erreur d'allocation de mémoire du thread de reception", LOG_PERROR);

                        /* Annulation des threads et libération de la mémoire. */
                        if(threadReception)
                        {
                            for (uint8_t i = 0; i < taille; i++)
                                pthread_cancel(threadReception[i]);
                            
                            free(threadReception);
                            threadReception = NULL;
                        }
                        
                        return NULL;
                    }

                    threadReception = threadReceptionTemp;
                    threadReceptionTemp = NULL;
                    indexClient = taille - 1;
                }
            }
        }
        else
        {
            printfSyslog("Le nombre de clients maximum est atteint.\n", LOG_INFO);
            
            send (client[indexClient].descripteurDeSocketClient, "Le nombre de clients maximum est atteint.\n", 50, 0);
            close (client[indexClient].descripteurDeSocketClient);
        }
    }

    for (uint8_t i = 0; i < taille; i++)
        verifJoinThread(threadReception, i);

    free(threadReception);
    free(client);
    threadReception = NULL;
    client          = NULL;

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
    Client*  client      = ((ArgumentThreadClient*)argumentThread)->client;
    uint8_t  indexClient = ((ArgumentThreadClient*)argumentThread)->indexClient;
    uint8_t* sortie      = ((ArgumentThreadClient*)argumentThread)->sortie;
    uint8_t sortieClient = 1;

    /* On récupère le buffer. */
    char buffer[LONGUEUR_BUFFER];

    while (sortieClient == 1 && *sortie == 1)
    {
        memset(buffer, 0, LONGUEUR_BUFFER);

        /* Variable d'indexage des clients. */
        printfSyslog("\nLecture de la requete : ", LOG_INFO);

        /* Lecture du message du client. */
        if (client)
        {
            recv(
                client[indexClient].descripteurDeSocketClient,
                buffer,
                LONGUEUR_BUFFER,
                0
            );

            printf("%s\n", buffer);
            syslog(LOG_INFO, "%s\n", buffer);

            printfSyslog("Lecture de la requete terminée.\n\n", LOG_INFO);

            /* On envoie la confirmation au client. */
            send(client[indexClient].descripteurDeSocketClient, "Requete lue.", 20, 0);

            // choix du client
            triChoix(
                client, 
                indexClient, 
                buffer, 
                sortie, 
                &sortieClient, 
                ((ArgumentThreadClient*)argumentThread)->taille, 
                ((ArgumentThreadClient*)argumentThread)->descripteurDeSocketServeur
            ); 
        }
    }
    return NULL;
}

/**
 * @brief C'est une fonction qui reçoit un client, un index du tableau client, un choix et un booléen. Il trie ensuite
 * le choix du client et exécute l'action correspondante
 * 
 * @param client la structure cliente
 * @param indexClient l'index du client dans le tableau client.
 * @param choix le choix du client
 * @param sortie un booléen indiquant la fermeture du serveur ou pas.
 * @param tailleMemoire la taille en mémoire du tableau client.
 * @param descripteurDesocketServeur
 */
void triChoix(Client* client, uint8_t indexClient, char* choix, uint8_t* sortie, uint8_t* sortieClient, unsigned long tailleMemoire, int descripteurDeSocketServeur)
{
    /* Exécute la commande ls et envoie le résultat au client. */
    if (strEq(choix, requeteLs))
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
    /*
     * Il reçoit une chaîne du client, la concatène avec la chaîne "partage/" puis change le répertoire
     * de travail courant en la désirée.
     */
    else if (strEq(choix, requeteCd))
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
    /* Permet au client de télécharger un fichier depuis le serveur.*/
    else if (strEq(choix, requeteDl))
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
    /* Reçoit un fichier du client et l'enregistre dans le système de fichiers du serveur. */
    else if (strEq(choix, requeteSend))
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
    else if (strEq(choix, requeteExit))
    {
        //*sortie = 0;
        
        // Fermeture de la connexion avec le client.
        close(client[indexClient].descripteurDeSocketClient);
        
        /* 
         * Définition du client à 0 dans le tableau, puis calcul de la taille du tableau client
         * moins la taille de la structure client pour ensuite vérifier si la valeur du résultat nvaleur
         * est inférieure ou égale à la valeur maximale d'un pointeur. Si c'est vrai, alors on réalloue
         * la mémoire du tableau client à la valeur calculer.
         */
        client[indexClient] = (Client){0};
        for (uint8_t i = indexClient; i + 1 < tailleMemoire / sizeof(Client); i++)
        {
            client[i] = client[indexClient + 1];
        }
        
        unsigned long nvaleur = tailleMemoire - sizeof(Client);
        if (nvaleur <= __PTRDIFF_MAX__)
        {
            Client* clientTemp = (Client*) realloc(client, nvaleur);

            if (!clientTemp)
            {
                printfSyslog("Erreur d'allocation de mémoire.\n", LOG_PERROR);
                client = NULL;
                exit(-1);
            }

            client = clientTemp;
            clientTemp = NULL;

            *sortieClient = 0;

            printfSyslog("Le client s'est bien deconnecté.\n", LOG_INFO);
        }
        else
        {
            printfSyslog("Erreur d'allocation de mémoire.\n", LOG_PERROR);
            free(client);
            client = NULL;
            exit(-1);
        }
    }
    else if (strEq(choix, requeteShutdown))
    {
        *sortie = 0;
        send(
            client->descripteurDeSocketClient,
            "Shutdown effectué.",
            LONGUEUR_BUFFER,
            0
        );
        /* Recalcul de la taille du tableau client. */
        uint8_t taille = tailleMemoire / sizeof(Client);

        /* Ferme le socket de chaque client. */
        for (int i = 0; i < taille - 1; i++)
            close(client[i].descripteurDeSocketClient);

        if(shutdown(descripteurDeSocketServeur, SHUT_RD) < 0)
            printfSyslog("Problème avec le shutdown", LOG_PERROR);
    }
}