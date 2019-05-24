#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <linux/limits.h>
#include <time.h>

#include "prensa.h"
#include "rwoper.h"

pid_t idExec, idLeg, idJud;
int daysLen, day = 0;

void signalHandler(int sig){
	if(sig == SIGUSR1){
		int ac = (random() % 100 < 50);
		char ans[2];
		if(ac) strcpy(ans, "Y");
		else strcpy(ans, "N");

		int fdToExec = open(JUD_EXEC_PIPE, O_WRONLY); 
		write(fdToExec, ans, 1);
		close(fdToExec);
	}
	if(sig == SIGUSR2){
		int ac = (random() % 100 < 50);
		char ans[2];
		if(ac) strcpy(ans, "Y");
		else strcpy(ans, "N");

		int fdToLeg = open(JUD_LEG_PIPE, O_WRONLY); 
		write(fdToLeg, ans, 1);
		close(fdToLeg);
	}
}

int main(int argc, char **argv){
	if(argc < 2){
		fprintf(stderr, "Too few arguments\n");
		return 0;
	}
	
	char action[MAX_ACTION][MAX_ACT_LINE];
	char planPath[PATH_MAX];
	char dir[PATH_MAX];

	// Number of days the simulation will run
	sscanf(argv[1], "%d", &daysLen);
	
	// Path of directory where Govt. files exist
	if(argc > 2)
		strcpy(dir, argv[2]);
	else
		dir[0] = '\0';

	// Path of Executive govt plan
	strcpy(planPath, dir);
	strcat(planPath, "Judicial.acc");

	// Pipe to read the other processes ID's
	int pfd = open(JUD_PIPE_NAME, O_RDONLY);
	char pids[100];
	read(pfd, pids, sizeof(pids));
	close(pfd);
	sscanf(pids, "%d %d %d", &idExec, &idLeg, &idJud);

	// Semaphore to sync in with press
	sem_t *syncSem = sem_open(PRESS_SYNC_SEM, O_CREAT, 0666, 0);

	// Set signal handler for passing of days and inform press
	signal(SIGUSR1, signalHandler);
	signal(SIGUSR2, signalHandler);

	sem_post(syncSem);
	sem_close(syncSem);

	// Pipe to comunicate with press
	pfd = open(PRESS_NAME, O_WRONLY);
	sem_t *syncSem2 = sem_open(PRESS_SYNC_SEM2, O_CREAT, 0666, 0);

	// Initialize random generator
	srandom(time(NULL));

	while(1){
		int nLines = 0; //readAction(planPath, action);
		if(nLines == 0){
			// Ninguna accion fue escogida
			char msg[150];
			sprintf(msg, JUD_IDDLE_MSG);
			writeToPress(pfd, msg, strlen(msg) + 1, syncSem);
		}
		/*else{
			int success = execAction(nLines, action, dir, idExec, idLeg, idJud);
			char msg[MAX_ACT_LINE];
			if(random() % 100 >= 66) success = 0;
			if(success){
				strcpy(msg, action[nLines-2] + 7);
				int sz = strlen(msg);
				writeToPress(pfd, msg, sz, syncSem2);		
			}
			else{
				strcpy(msg, action[nLines-1] + 9);
				int sz = strlen(msg);
				writeToPress(pfd, msg, sz, syncSem2);
			}
		}*/
	}

	// Close pipe and semaphore
	close(pfd);
	sem_close(syncSem2);

}