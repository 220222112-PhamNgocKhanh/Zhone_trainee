CC = gcc

TARGET = producer_customer

SRC = doubly_link_list.c queue.c producer_customer.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)