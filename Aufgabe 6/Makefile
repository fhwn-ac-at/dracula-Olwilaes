CC = clang
CFLAGS = -Wall -Wextra -Werror -std=c99 -Isrc
TARGET = snakesAndLaddersSimulator
SRCDIR = src
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/game.c
HEADERS = $(SRCDIR)/game.h
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

$(SRCDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o $(TARGET)

# Run with test board
test: $(TARGET)
	./$(TARGET) testboard.txt

# Check for memory leaks (requires valgrind)
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) testboard.txt