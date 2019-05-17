#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
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

	int idExec, idLeg, idJud;

	// Init Executive
	if((idExec = fork()) == 0){
		char *nargv[] = { "./ejecutivo.o", dir , NULL };
		execvp(nargv[0], nargv);
	}

	// Init Legislative
	if((idExec = fork()) == 0){
		char *nargv[] = { "./legislativo.o", dir , NULL };
		execvp(nargv[0], nargv);
	}

	// Init Judicial
	if((idExec = fork()) == 0){
		char *nargv[] = { "./judicial.o", dir , NULL };
		execvp(nargv[0], nargv);
	}

	
	
}