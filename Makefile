CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
SRC = src
BIN = bin

SOURCES = $(wildcard $(SRC)/*.c)
OBJECTS = $(SOURCES:.c=.o)
TARGET = $(BIN)/6502-emulator

all: $(TARGET)

$(TARGET): $(OBJECTS)
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN) $(SRC)/*.o

run: all
	./$(TARGET)
