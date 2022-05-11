#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

// permet de comparer 2 chaines de character
int compare_char(const char *s1, const char *s2)
{
    while (*s1 == *s2 && *s1)
    {
        s1++;
        s2++;
    }
    return *s1 == *s2;
}

int main(int argc, char const *argv[])
{

    char mot[20];
    char choixFichier[20];
    char *ls = "ls partage > resultat.txt"; //fait
    char *cd = "cd";     // fait
    char *send = "send"; // code client socket
    char *dl = "dl";     // code serveur socket
    char *exit = "exit"; // fait
    char *help = "help"; // fait
    bool exitCondition = false;

    system("clear");
    printf("Voici les commandes que vous pouvez utiliser : \nls\n%s\n%s\n%s\n%s\n", cd, send, help, exit);
    // client
    do
    {
        printf("Ecrire une commande : \n");
        scanf("%s", mot);

        if (compare_char(mot, "ls"))
        {
            /* Effacer l'écran, puis exécuter la commande ls, puis ouvrir le fichier resultat.txt, puis
            lire le fichier et l'imprimer, puis fermer le fichier, puis supprimer le fichier. */
            system("clear");

            //serv
            system(ls);

            //client
            FILE *f;
            char c;
            f = fopen("resultat.txt", "r");
            if (f)
            {
                while ((c = fgetc(f)) != EOF)
                {
                    printf("%c", c);
                }
                fclose(f);
            }
            else printf("NON");

            //serv
            system("rm -f resultat.txt");
        }
        else if (compare_char(mot, cd))
        {
            /* Il demande à l'utilisateur de choisir un dossier, puis il change le répertoire vers le dossier
            choisi. */
            system("clear");
            printf("Dans quel dossier voulez-vous aller ? \n");
            system("ls partage");
            scanf("%s", choixFichier);

            char *cdDossier = malloc(sizeof(choixFichier) + 12);
            strcat(cdDossier, "partage/");
            strcat(cdDossier, choixFichier);

            printf("Vous avez choisis le dossier : %s \n", choixFichier);
            chdir(cdDossier);
            free (cdDossier);
        }
        else if (compare_char(mot, send))
        {
            system("clear");
            printf("send...\n");
        }
        else if (compare_char(mot, help))
        {
            /* Impression de la liste des commandes que l'utilisateur peut utiliser. */
            system("clear");
            printf("Voici les commandes que vous pouvez utiliser : \nls\n%s\n%s\n%s\n%s\n", cd, send, help, exit);
        }
        else if (compare_char(mot, exit))
        {
           /* Une variable booléenne utilisée pour quitter le programme. */
            exitCondition = true;
        }
        else
        {
            printf("la commande %s n'existe pas \n", mot);
        }
    } while (exitCondition == false);
    printf("Sortie du porgramme... \n");

    return 0;
}