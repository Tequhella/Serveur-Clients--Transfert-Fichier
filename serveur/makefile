CC = gcc

GCC = -pthread -D_REENTRANT -Wall


all: build/serveur.o build/fonction.o
	$(CC) $(GCC) build/serveur.o build/fonction.o -o serveur

build/serveur.o:
	$(CC) $(GCC) -c serveur.c -o build/serveur.o

build/fonction.o:
	$(CC) $(GCC) -c fonction.c -o build/fonction.o

clean:
	rm -f serveur build/serveur.o build/fonction.o