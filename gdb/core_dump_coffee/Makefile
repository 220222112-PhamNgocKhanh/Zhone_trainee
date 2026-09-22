CC = gcc
CFLAGS = -Wall -Wextra -g -O0 -std=c99

TARGET = cafe_pos
SRCS = main.c menu_catalog.c order_manager.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS) core core.* *.o

.PHONY: all clean

	