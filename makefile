CC = gcc
CFLAGS = -pthread -std=gnu99
DEPS = prensa.h rwoper.h
OBJ = exec.o legis.o judi.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

paisProcesos: prensa.c $(OBJ)
	$(CC) -o $@ $< $(CFLAGS)

.PHONY: clean

clean: 
	rm paisProcesos
	rm *.o