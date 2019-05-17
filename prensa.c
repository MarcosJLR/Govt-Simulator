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

#include "prensa.h"

int main(int argc, char **argv){
	if(argc < 2){
		fprintf(stderr, "Too few arguments\n");
		return 0;
	}
	
	// Number of days the simulation will run
	int daysLen, day = 0;
	sscanf(argv[1], "%d", &daysLen);
	
	// Path of directory where Govt. files exist
	char dir[PATH_MAX];
	dir[0] = '\0';
	if(argc > 2)
		strcpy(dir, argv[2]);

	// Named Pipe to retrieve headlines
	mkfifo(PRESS_NAME, 0666);

	// Syncronization semaphore
	sem_unlink(PRESS_SYNC_SEM);
	sem_t *syncSem = sem_open(PRESS_SYNC_SEM, O_CREAT, 0666, 0);
	if(syncSem == NULL){
		fprintf(stderr, "Failed to open Semaphore\n");
		return 0;
	}

	pid_t idExec, idLeg, idJud;

	// Init Executive
	if((idExec = fork()) == 0){
		char *nargv[] = { "./exec.o", daysLen, dir , NULL };
		execvp(nargv[0], nargv);
	}

	// Init Legislative
	if((idExec = fork()) == 0){
		char *nargv[] = { "./legis.o", daysLen, dir , NULL };
		execvp(nargv[0], nargv);
	}

	// Init Judicial
	if((idExec = fork()) == 0){
		char *nargv[] = { "./judi.o", daysLen, dir , NULL };
		execvp(nargv[0], nargv);
	}

	// Here starts the press work
	// Buffer to read from pipe
	char buf[PIPE_BUF];

	// Pipe file descriptor
	int pfd = open(PRESS_NAME, O_RDONLY);

	while(day < daysLen){
		day++;

		// Reads from pipe and signals semaphore
		// to inform writer that his input has been read
		int nBytes = read(pfd, buf, sizeof(buf));
		sem_post(syncSem);
		if(nBytes == 0){
			fprintf(stderr, "All writers have closed their pipes\n");
			break;
		}

		// Publish headline
		printf("Dia %d: %s\n", day, buf);

		// Send signal to processes informing a day has passed
		kill(idExec, SIGUSR1);
		kill(idLeg, SIGUSR1);
		kill(idJud, SIGUSR1);
	}
	
	// Close pipe and semaphore
	close(pfd);
	sem_close(syncSem);

	// Wait for the other processes to terminate
	int status;
	waitpid(idExec, &status, 0);
	waitpid(idLeg, &status, 0);
	waitpid(idJud, &status, 0);

	// Unlink semaphore
	sem_unlink(PRESS_SYNC_SEM);

	return 0;
}