/**
 * @file baseFonction.c
 * @author Lilian CHARDON (lilian.chardon@ynov.com)
 * @brief 
 * @version 0.1
 * @date 19-10-2023
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include "baseFonction.h"

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

    // vide le buffer de sortie
    fflush(stdout);
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

