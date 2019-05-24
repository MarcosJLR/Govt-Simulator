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
#include <pthread.h> 

#include "prensa.h"
#include "rwoper.h"

char action[MAX_ACTION][MAX_ACT_LINE];
pid_t idExec, idLeg, idJud;
int daysLen, day = 0;
char dir[PATH_MAX];
char planPath[PATH_MAX];

void signalHandler(int sig, siginfo_t *info, void *ucontext){
	if(sig == SIGUSR1){
		day++;
		if(day >= daysLen){
			exit(sig);
		}
	}
	if(sig == SIGUSR2){
		int ac = (random() % 100 < 50);
		char ans[2];
		if(ac)
			strcpy(ans, "Y");
		else
			strcpy(ans, "N");
		if(info->si_pid == idExec){
			int fdToExec = open(JUD_EXEC_PIPE, O_WRONLY); 
			write(fdToExec, ans, 1);
			close(fdToExec);
		}
		if(info->si_pid == idLeg){
			int fdToLeg = open(JUD_LEG_PIPE, O_WRONLY); 
			write(fdToLeg, ans, 1);
			close(fdToLeg);
		}
	}
}

void* judi(void *arg){
	struct ThreadArguments *ta = arg;
	
	// Number of days the simulation will run
	sscanf(argv[1], "%d", &daysLen);
	
	// Path of directory where Govt. files exist
	if(argc > 2)
		strcpy(dir, argv[2]);

	// Path of Executive govt plan
	if(strcmp(dir, "\0")==0){
		strcpy(planPath, "Judicial.acc");
	}
	else{
		strcpy(planPath, dir);
		strcat(planPath, "/Judicial.acc");
	}

	// Pipe to read the other processes ID's
	int pfd = open(JUD_PIPE_NAME, O_RDONLY);
	char pids[100];
	read(pfd, pids, sizeof(pids));
	close(pfd);
	sscanf(pids, "%d %d %d", &idExec, &idLeg, &idJud);

	// Semaphore to sync in with press
	sem_t *syncSem = sem_open(PRESS_SYNC_SEM, O_CREAT, 0666, 0);

	// Set signal handler for passing of days and inform press
	struct sigaction *sigH = malloc(sizeof(struct sigaction));
	sigH->sa_sigaction = &signalHandler;
	sigH->sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, sigH, NULL);
	sigaction(SIGUSR2, sigH, NULL);
	sem_post(syncSem);
	free(sigH);
	sem_close(syncSem);

	// Pipe to comunicate with press
	pfd = open(PRESS_NAME, O_WRONLY);
	sem_t *syncSem2 = sem_open(PRESS_SYNC_SEM2, O_CREAT, 0666, 0);

	// Initialize random generator
	srandom(time(NULL));

	while(1){
		int nLines = readAction(planPath, action);
		if(nLines == 0){
			// Ninguna accion fue escogida
			char msg[100];
			//strcpy(msg, JUD_IDDLE_MSG);
			sprintf(msg,"Hola del Tribunal, proceso %d\n", idJud);
			//writeToPress(pfd, msg, strlen(msg), syncSem);
			flock(pfd, LOCK_EX);
			write(pfd, msg, strlen(msg));
			sem_wait(syncSem2);
			flock(pfd, LOCK_UN);
		}
		else{
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
		}
	}

	// Close pipe and semaphore
	close(pfd);
	sem_close(syncSem2);

}