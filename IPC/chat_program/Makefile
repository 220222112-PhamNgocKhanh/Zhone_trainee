CC = gcc
CFLAGS = -Wall -Wextra -pthread

TARGETS = client1 client2

all: $(TARGETS)

client1: client1.c
	$(CC) $(CFLAGS) client1.c -o client1

client2: client2.c
	$(CC) $(CFLAGS) client2.c -o client2

clean:
	rm -f $(TARGETS) /tmp/chat_fifo_1_to_2 /tmp/chat_fifo_2_to_1

.PHONY: all clean

