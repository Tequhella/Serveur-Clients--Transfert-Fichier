/**
 * @file baseFonction.h
 * @author Lilian CHARDON, Andréas Castello (lilian.chardon@ynov.com, andreas.castello@ynov.com)
 * @brief 
 * @version 0.1
 * @date 19-10-2023s
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef BASE_FONCTION_H
#define BASE_FONCTION_H

#include "global.h"

// -lpthread
// -D_REENTRANT



/**
 * @brief fonction itoa, convertit un entier en une chaîne de caractères.
 * 
 * @param val : entier à convertir.
 * @param base : base de la conversion.
 * @return char* : chaîne de caractères correspondant à l'entier.
 */
char* itoa(int val, int base);

/**
 * @brief fonction strEq, test légalité de 2 chaînes de caractères.
 * 
 * @param str1 La première chaîne à comparer.
 * @param str2 La chaîne à comparer.
 * 
 * @return 1 si les deux chaînes sont égales, 0 sinon.
 */
uint8_t strEq(const char* str1, const char* str2);

/**
 * @brief fonction printfSyslog, imprime la chaîne donnée sur la console et via le système de journalisation.
 * 
 * @param str La chaîne à imprimer.
 * @param logType Le type de log.
 */
void printfSyslog(const char* str, const uint8_t logType);


/**
 * @brief fonction verifJoinThread, vérifie la fin de fonctionnement du thread.
 * 
 * @param thread Le thread donné à vérifier.
 * @param index Index du tableau de thread si c'en est un.
 * 
 */
void verifJoinThread(pthread_t* thread, uint8_t index);

#endif