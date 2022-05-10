#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int my_streq(const char *s1, const char *s2) {
  while (*s1 == *s2 && *s1) {
    s1++;
    s2++;
  }
  return *s1 == *s2;
}

int main(int argc, char const *argv[])
{

    char mot [20];
    char* ls = "ls partage";

//client
    printf("Ecrire une commande : \n");
    scanf("%s", mot);

//serv
    if(my_streq(mot, "ls"))
    {
        system("clear");
        system(ls);
    }
    else printf("pas ls \n");

//client
    return 0;
}
