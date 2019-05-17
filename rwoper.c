#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <linux/limits.h>
#include <sys/file.h>

#include "prensa.h"
#include "rwoper.h"

int writeToPress(int fd, char *buf, int nBytes, sem_t *syncSem){
	if(nBytes >= PIPE_BUF){
		fprintf(stderr, "Headline is too long!\n");
		return 0;
	}

	flock(fd, LOCK_EX);
	int wBytes = write(fd, buf, nBytes);
	sem_wait(syncSem);
	flock(fd, LOCK_UN);

	return wBytes;
}
