CC=gcc

CFLAGS=-O3 -Wall -Wextra -fsanitize=address -fsanitize=undefined

.PHNOY: all
all: gammacorrect
gammacorrect: gammacorrect.c
	$(CC) $(CFLAGS) -o $@ $^ 

.PHNOY: debug
debug: gammacorrect.c
	$(CC) -g $(CFLAGS) -o $@ $^

.PHNOY: clean
clean:
	rm -f gammacorrect debug