/**
 * @file global.h
 * @author Lilian CHARDON (lilian.chardon@ynov.com)
 * @brief Fichier .h contenant toutes les constantes et variables globales
 * @version 0.1
 * @date 10-05-2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <syslog.h>
#include <errno.h>
#include <sys/stat.h>

/*
 * À ne pas en prendre en compte dans les modifications.
 * Décommenter uniquement pour éviter les bugs de l'éditeur de texte.
 */
#undef NULL                          // Pour éviter les erreurs de compilation
#define NULL (void*)0                // Valeur NULL

#define LONGUEUR_BUFFER 1024         // Taille du buffer de lecture
#define ipMachine "127.0.0.1"        // Définition de l'ip du serveur avec la valeur 127.0.0.1.
#define PORT 2000                    // Il définit le numéro de port à 2000.
#define requeteConnectee "connectee" // Il définit la requête à envoyer au serveur.
#define requeteLs       "ls -C partage > resultat.txt"
#define requeteCd       "cd"
#define requeteSend     "send"
#define requeteDl       "dl"
#define requeteExit     "exit"
#define requeteHelp     "help"
#define requeteShutdown "shutdown"

#endif