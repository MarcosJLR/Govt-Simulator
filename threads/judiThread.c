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
	
	// Path of Executive govt plan
	if(strcmp(ta->dir, "\0")==0){
		strcpy(planPath, "Judicial.acc");
	}
	else{
		strcpy(planPath, ta->dir);
		strcat(planPath, "/Judicial.acc");
	}


	// Initialize random generator
	srandom(time(NULL));

	while(ta->end){
		int nLines = readAction(planPath, action);
		if(nLines == 0){
			// Ninguna accion fue escogida
			char msg[150];
			//strcpy(msg, EXEC_IDDLE_MSG);
			sprintf(msg,"Hola del Tribunal");
			//writeToPress(pfd, msg, strlen(msg), syncSem);
			sem_wait(&ta->write);
			sem_wait(&ta->full);
			strcpy(ta->headline[ta->head], msg);
			ta->head=(ta->head+1)%10;
			sem_post(&ta->press);
			sem_post(&ta->write);
		}
		else{
			int success = execAction(nLines, action, dir, idExec, idLeg, idJud);
			char msg[MAX_ACT_LINE];
			if(random() % 100 >= 66) success = 0;
			if(success){
				strcpy(msg, action[nLines-2] + 7);
				int sz = strlen(msg);
				writeToPress(&ta, msg, sz);		
			}
			else{
				strcpy(msg, action[nLines-1] + 9);
				int sz = strlen(msg);
				writeToPress(&ta, msg, sz);
			}
		}
	}
	return 0;

}