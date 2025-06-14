CC = clang
CFLAGS = -Wall -Wextra -Werror
SRC = src/main.c src/interpreter.c src/visualizer.c src/hashTable.c
OUT = pfusch

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)

.PHONY: all clean