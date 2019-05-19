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
#include "rwoper.h"

char action[MAX_ACTION][MAX_ACT_LINE];
pid_t idExec, idLeg, idJud;
int daysLen, day = 0;
char dir[PATH_MAX];
char planPath[PATH_MAX];

void signalHandler(int sig){
	if(sig == SIGUSR1){
		day++;
		if(day >= daysLen){
			exit(sig);
		}
	}
}

int main(int argc, char **argv){
	if(argc < 2){
		fprintf(stderr, "Too few arguments\n");
		return 0;
	}
	
	// Number of days the simulation will run
	sscanf(argv[1], "%d", &daysLen);
	
	// Path of directory where Govt. files exist
	if(argc > 2)
		strcpy(dir, argv[2]);

	// Path of Executive govt plan
	strcpy(planPath, dir);
	strcat(planPath, "/Ejecutivo.acc");

	// Pipe to read the other processes ID's
	int pfd = open(EXEC_PIPE_NAME, O_RDONLY);
	char pids[100];
	read(pfd, pids, sizeof(pids));
	close(pfd);
	sscanf(pids, "%d %d %d", &idExec, &idLeg, &idJud);

	// Semaphore to sync in with press
	sem_t *syncSem = sem_open(PRESS_SYNC_SEM, O_CREAT, 0666, 0);

	// Set signal handler for passing of days and inform press
	signal(SIGUSR1, signalHandler);
	sem_post(syncSem);

	// Pipe to comunicate with press
	pfd = open(PRESS_NAME, O_WRONLY);

	// Initialize random generator
	srandom(time(NULL));

	while(1){
		int nLines = readAction(planPath, action);
		if(nLines == 0){
			// Ninguna accion fue escogida
			char *msg = EXEC_IDDLE_MSG;
			writeToPress(pfd, msg, sizeof(msg), syncSem);
		}
		else{
			int success = execAction(nLines, action);
			char msg[MAX_ACT_LINE];
			if(random() % 100 >= 66) success = 0;
			if(success){
				strcpy(msg, action[n-2] + 7);
				int sz = strlen(msg);
				writeToPress(pfd, msg, sz, syncSem);		
			}
			else{
				strcpy(msg, action[n-1] + 9);
				int sz = strlen(msg);
				writeToPress(pfd, msg, sz, syncSem);
			}
		}
	}

	// Close pipe and semaphore
	close(pfd);
	sem_close(syncSem);

}