PROJECT = pcm2wav
CC = gcc
CFLAGS = -Wall
RM = rm -f

PROG1 = $(PROJECT)
PROG2 = wav2pcm
PROGS = $(PROG1) $(PROG2)

all: $(PROGS)

$(PROG1): src/$(PROG1).c
	$(CC) $(CFLAGS) $^ -o $@

$(PROG2): src/$(PROG2).c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) $(PROGS)
