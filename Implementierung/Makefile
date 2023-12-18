CC=gcc

CFLAGS=-O3 -Wall -Wextra -fsanitize=address -fsanitize=undefined#valgrind reports error if -fsanitize=address

.PHNOY: all
all: gammacorrect
gammacorrect: gammacorrect.c readppm.c
	$(CC) $(CFLAGS) -o $@ $^ 

.PHNOY: debug
debug: gammacorrect.c readppm.c
	$(CC) -g $(CFLAGS) -o $@ $^

.PHNOY: clean
clean:
	rm -f gammacorrect debug