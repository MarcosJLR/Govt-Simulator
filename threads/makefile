CC = gcc
CFLAGS = -pthread -std=gnu99
DEPS = prensaThread.h rwoperThread.h execThread.h legisThread.h judiThread.h
SRC = prensaThread.c rwoperThread.c execThread.c legisThread.c judiThread.c

paisHilos: $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean: 
	rm paisHilos