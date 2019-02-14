CC=gcc
CFLAGS=-W -Wall
LDFLAGS=
EXEC=executable

SRCC=MT19937.c untwist.c
SRCH= $(SRCC:.c=.h)
SRC= $(SRCC) main.c
OBJ= $(SRC:.c=.o)

all: $(EXEC)

executable: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

main.o: $(SRCH)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)