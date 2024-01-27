CC=gcc

CFLAGS=-O3 -lm -Wall -Wextra -fsanitize=address -fsanitize=undefined#valgrind reports error if -fsanitize=address

.PHNOY: all
all: gammacorrect
gammacorrect: gammacorrect.c readppm.c V0.c V1.c V2.c
	$(CC) $(CFLAGS) -o $@ $^ 

.PHNOY: debug
debug: gammacorrect.c readppm.c V0.c V1.c V2.c
	$(CC) -g $(CFLAGS) -o $@ $^

.PHNOY: clean
clean:
	rm -f gammacorrect debug