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

pid_t idExec, idLeg, idJud;
int daysLen, day = 0;
char dir[PATH_MAX];

int main(int argc, char **argv){
	// Needs to have at least the duration of the simulation
	if(argc < 2){
		fprintf(stderr, "Too few arguments\n");
		return 0;
	}
	
	// Number of days the simulation will run
	sscanf(argv[1], "%d", &daysLen);
	
	// Path of directory where Govt. files exist
	if(argc > 2)
		strcpy(dir, argv[2]);

	// Named Pipe to retrieve headlines
	mkfifo(PRESS_NAME, 0666);

	// Named Pipes to send info to child processes
	mkfifo(EXEC_PIPE_NAME, 0666);
	mkfifo(LEGIS_PIPE_NAME, 0666);
	mkfifo(JUD_PIPE_NAME, 0666);

	// Syncronization semaphore
	sem_unlink(PRESS_SYNC_SEM);
	sem_t *syncSem = sem_open(PRESS_SYNC_SEM, O_CREAT, 0666, 0);
	if(syncSem == NULL){
		fprintf(stderr, "Failed to open Semaphore\n");
		return 0;
	}

	// Init Executive
	if((idExec = fork()) == 0){
		char *nargv[] = { "./exec.o", argv[1], dir , NULL };
		execvp(nargv[0], nargv);
	}

	// Init Legislative
	if((idLeg = fork()) == 0){
		char *nargv[] = { "./legis.o", argv[1], dir , NULL };
		execvp(nargv[0], nargv);
	}

	// Init Judicial
	if((idJud = fork()) == 0){
		char *nargv[] = { "./judi.o", argv[1], dir , NULL };
		execvp(nargv[0], nargv);
	}

	// Passes the Process ID of the three children to each of them
	int pfd;
	char pids[100];
	sprintf(pids, "%d %d %d\n", idExec, idLeg, idJud);
	int pidsLen = strlen(pids);

	pfd = open(EXEC_PIPE_NAME, O_WRONLY);
	write(pfd, pids, pidsLen);
	close(pfd);

	pfd = open(LEGIS_PIPE_NAME, O_WRONLY);
	write(pfd, pids, pidsLen);
	close(pfd);
	
	pfd = open(JUD_PIPE_NAME, O_WRONLY);
	write(pfd, pids, pidsLen);
	close(pfd);
	
	// Wait for children to set the signal handler for 
	// the passing of days
	sem_wait(PRESS_SYNC_SEM);
	sem_wait(PRESS_SYNC_SEM);
	sem_wait(PRESS_SYNC_SEM);

	// Here starts the press work
	// Buffer to read from pipe
	char buf[PIPE_BUF];

	// Pipe file descriptor
	pfd = open(PRESS_NAME, O_RDONLY);

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

	// Unlink semaphore and delete pipes
	sem_unlink(PRESS_SYNC_SEM);
	char rmCom[100];
	strcpy(rmCom, "rm ");
	strcat(rmCom, PRESS_NAME);
	system(rmCom);
	strcpy(rmCom, "rm ");
	strcat(rmCom, EXEC_PIPE_NAME);
	system(rmCom);
	strcpy(rmCom, "rm ");
	strcat(rmCom, LEGIS_PIPE_NAME);
	system(rmCom);
	strcpy(rmCom, "rm ");
	strcat(rmCom, JUD_PIPE_NAME);
	system(rmCom);

	return 0;
}