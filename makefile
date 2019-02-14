CC=gcc
CFLAGS=-W -Wall
LDFLAGS=
EXEC=executable
SRC= MT19937.c main.c
OBJ= $(SRC:.c=.o)

all: $(EXEC)

executable: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

main.o: MT19937.h

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)