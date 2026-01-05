CC = gcc

CFLAGS = -Wall -std=c99

LDFLAGS = -lraylib -lGL -lm -lpthread -ldl

TARGET = symulator

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)