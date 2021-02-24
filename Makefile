CC = gcc
FLAGS = `pkg-config --libs gtk+-3.0` -lm -Wall -Wextra -std=c99 -march=native -rdynamic 
GTK = `pkg-config --cflags gtk+-3.0` -ggdb3 -g
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
EXEC = OCR

all: $(EXEC)

%.o : %.c
	$(CC) $(GTK) -o $@ -c $<

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(FLAGS)

clean:
	rm src/*.o

cleanall: clean
	rm $(EXEC)
