CC=gcc

CFLAGS=-O3 -Wall -Wextra -fanalyzer

.PHNOY: all
all: gammacorrect
gammacorrect: gammacorrect.c
	$(CC) $(CFLAGS) -o $@ $^ 

.PHNOY: debug
debug: gammacorrect.c
	$(CC) -g $(CFLAGS) -o $@ $^


.PHNOY: clean
clean:
	rm -f gammacorrect