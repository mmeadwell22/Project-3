CC=gcc -g -Wall
CFLAGS=-I.
DEPS = boot.h Fat12Boot.h Fat12Entry.h
OBJ = main.o boot.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
proj3: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o proj3