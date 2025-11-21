CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -pthread `pkg-config --cflags libcurl`
LDFLAGS = `pkg-config --libs libcurl` -lpthread
SRC = src/main.c src/crawler.c src/url_queue.c src/visited.c src/fetcher.c src/parser.c src/storage.c
OBJ = $(SRC:.c=.o)
TARGET = crawler

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
