CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra -D_GNU_SOURCE

TARGET = benchmark
SRCS = benchmark.c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
