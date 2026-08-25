CC = gcc
CFLAGS = -Wall -lm

all: particles

particles: particles.o
	$(CC) $(CFLAGS) $^ -lm -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean

clean:
	rm *.o
