GCC = gcc

CFLAGS = -pthread -D_REENTRANT -Wall -Iheaders

LBUILD = -Lbuild -lexplain

S_DIR = sources/

H_DIR = headers/

B_DIR = build/


all: $(B_DIR)serveur.o $(B_DIR)reseau.o $(B_DIR)baseFonction.o $(B_DIR)gestionClient.o $(B_DIR)triChoix.o
	$(GCC) $(CFLAGS) $(B_DIR)serveur.o $(B_DIR)reseau.o $(B_DIR)baseFonction.o $(B_DIR)gestionClient.o $(B_DIR)triChoix.o -o serveur $(LBUILD)

$(B_DIR)serveur.o: $(S_DIR)serveur.c $(H_DIR)/reseau.h $(H_DIR)/baseFonction.h $(H_DIR)/gestionClient.h
	$(GCC) $(CFLAGS) -c $(S_DIR)serveur.c -o $(B_DIR)serveur.o

$(B_DIR)reseau.o: $(S_DIR)reseau.c $(H_DIR)/reseau.h
	$(GCC) $(CFLAGS) -c $(S_DIR)reseau.c -o $(B_DIR)reseau.o

$(B_DIR)baseFonction.o: $(S_DIR)baseFonction.c $(H_DIR)/baseFonction.h
	$(GCC) $(CFLAGS) -c $(S_DIR)baseFonction.c -o $(B_DIR)baseFonction.o

$(B_DIR)gestionClient.o: $(S_DIR)gestionClient.c $(H_DIR)/gestionClient.h
	$(GCC) $(CFLAGS) -c $(S_DIR)gestionClient.c -o $(B_DIR)gestionClient.o $(LBUILD)

$(B_DIR)triChoix.o: $(S_DIR)triChoix.c $(H_DIR)/triChoix.h
	$(GCC) $(CFLAGS) -c $(S_DIR)triChoix.c -o $(B_DIR)triChoix.o $(LBUILD)

dep:
	sudo apt install libexplain-dev

clean:
	rm -f serveur $(B_DIR)*.o
