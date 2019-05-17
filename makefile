CC = gcc
CFLAGS = -pthread -std=gnu99 -I
DEPS = prensa.h rwoper.h
OBJ = rwoper.o ejecutivo.o legislativo.o judicial.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

paisProcesos: prensa.c $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean: 
	rm paisProcesos
	rm *.o