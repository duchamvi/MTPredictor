CC=gcc
CFLAGS=-W -Wall
LDFLAGS=
EXEC=mtpredictor.out

SRCC=MT19937.c untwist.c
SRCH= $(SRCC:.c=.h)
SRC= $(SRCC) main.c
OBJ= $(SRC:.c=.o)

SRCTUTO= $(SRCC) tuto.c
OBJTUTO= $(SRCTUTO:.c=.o)
tuto=tuto.out

all: $(EXEC)

mtpredictor.out: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)


tuto.out: $(OBJTUTO)
	$(CC) -o $@ $^ $(LDFLAGS)

main.o: $(SRCH)
tuto.o: $(SRCH)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)