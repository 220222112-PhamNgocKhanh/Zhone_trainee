CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -g -pthread -Wno-unused-result
TARGET  = chat_server
SRCS    = server.c

all: $(TARGET)

$(TARGET): $(SRCS) server.h
	$(CC) $(CFLAGS) -o $@ $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
