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

#define LONGUEUR_BUFFER 1024         // Taille du buffer de lecture

#define ipMachine "127.0.0.1"        // Définition de l'ip du serveur avec la valeur 127.0.0.1.
#define PORT 2000                    // Il définit le numéro de port à 2000.
#define requeteConnectee "connectee" // Il définit la requête à envoyer au serveur.
#define requeteLs       "ls partage > resultat.txt"
#define requeteCd       "cd"
#define requeteSend     "send"
#define requeteDl       "dl"
#define requeteExit     "exit"
#define requeteHelp     "help"
#define requeteShutdown "shutdown"

#endif