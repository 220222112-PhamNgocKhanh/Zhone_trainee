CC = gcc
CFLAGS = -Wall -Wextra

TARGETS = producer consumer1 consumer2 consumer3

all: $(TARGETS)

consumer1: consumer1.c
	$(CC) $(CFLAGS) consumer1.c -o consumer1

consumer2: consumer2.c
	$(CC) $(CFLAGS) consumer2.c -o consumer2

consumer3: consumer3.c
	$(CC) $(CFLAGS) consumer3.c -o consumer3

producer: producer.c
	$(CC) $(CFLAGS) producer.c -o producer

clean:
	rm -f $(TARGETS)

.PHONY: all clean
