CC = gcc

GCC = -pthread -D_REENTRANT


all: build/client.o build/fonction.o
	$(CC) $(GCC) build/client.o build/fonction.o -o client

build/client.o:
	$(CC) $(GCC) -c client.c -o build/client.o

build/fonction.o:
	$(CC) $(GCC) -c fonction.c -o build/fonction.o

dep:
	sudo apt install libexplain-dev

clean:
	rm -f client build/client.o build/fonction.o
