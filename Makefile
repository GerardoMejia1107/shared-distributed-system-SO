CC = gcc
CFLAGS = -Wall -Iinclude

my_dsm: src/main.c src/dsm.c include/dsm.h
	$(CC) $(CFLAGS) src/main.c src/dsm.c -o my_dsm

clean:
	rm -f my_dsm
