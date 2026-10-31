CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

my_dsm: src/main.c src/dsm.c src/net.c include/dsm.h include/net.h
	$(CC) $(CFLAGS) src/main.c src/dsm.c src/net.c -o my_dsm

clean:
	rm -f my_dsm
