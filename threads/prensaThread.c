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
#include <errno.h>
#include <pthread.h> 

#include "prensa.h"


struct ThreadArguments{
	char headline[500];
	char exec[500], judi[500], legis[500];
	char exec_leg[500], exec_jud[500];
	char jud_leg[500], jud_exec[500];
	char leg_exec[500], leg_jud[500];
	char dir[PATH_MAX];
	int days;
	pthread_t *exec, *legis, *judi;
};

int main(int argc, char **argv){
	int daysLen, day = 0;
	pthread_t exec, legis, judi;
	char dir[PATH_MAX];
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

	// Creating a struct with arguments pssed to thread
	struct ThreadArguments ta;
	strcpy(ta.dir, dir);
	ta.days=daysLen;
	ta.exec=&exec;
	ta.judi=&judi;
	ta.legis=&legis;
	// Named Pipe to retrieve headlines
	//mkfifo(PRESS_NAME, 0666);

	// Named Pipes to send info to child processes
	//mkfifo(EXEC_PIPE_NAME, 0666);
	//mkfifo(LEGIS_PIPE_NAME, 0666);
	//mkfifo(JUD_PIPE_NAME, 0666);

	// Named pipes for sibling comunication
	//mkfifo(EXEC_LEG_PIPE, 0666);
	//mkfifo(EXEC_JUD_PIPE, 0666);

	//mkfifo(LEG_EXEC_PIPE, 0666);
	//mkfifo(LEG_JUD_PIPE, 0666);

	//mkfifo(JUD_EXEC_PIPE, 0666);
	//mkfifo(JUD_LEG_PIPE, 0666);


	// Syncronization semaphore
	sem_unlink(PRESS_SYNC_SEM);
	sem_unlink(PRESS_SYNC_SEM2);
	sem_t *syncSem = sem_open(PRESS_SYNC_SEM, O_CREAT, 0666, 0);
	sem_t *syncSem2 = sem_open(PRESS_SYNC_SEM2, O_CREAT, 0666, 0);
	//sem_t *syncSem = sem_open(PRESS_SYNC_SEM, O_CREAT, 0666, 0);
	//if(syncSem == NULL){
	//	fprintf(stderr, "Failed to open Semaphore\n");
	//	return 0;
	//}

	// Init Executive
	pthread_create(&exec, NULL, execThread, &ta); 
	//if((idExec = fork()) == 0){
	//	char *nargv[] = { "./exec.o", argv[1], dir , NULL };
	//	if(execvp(nargv[0], nargv) == -1){
	//		fprintf(stderr, "Failed to execute %s because %s\n", nargv[0], strerror(errno));
	//		exit(0);
	//	}
	//}

	// Init Legislative
	pthread_create(&legis, NULL, legisThread, &ta);
	//if((idLeg = fork()) == 0){
	//	char *nargv[] = { "./legis.o", argv[1], dir , NULL };
	//	if(execvp(nargv[0], nargv) == -1){
	//		fprintf(stderr, "Failed to execute %s because %s\n", nargv[0], strerror(errno));
	//		exit(0);
	//	}
	//}

	// Init Judicial
	pthread_create(&judi, NULL, judiThread, &ta);
	//if((idJud = fork()) == 0){
	//	char *nargv[] = { "./judi.o", argv[1], dir , NULL };
	//	if(execvp(nargv[0], nargv) == -1){
	//		fprintf(stderr, "Failed to execute %s because %s\n", nargv[0], strerror(errno));
	//		exit(0);
	//	}
	//}

	// Passes the Process ID of the three children to each of them
	//int pfd;
	//char pids[100];
	//sprintf(pids, "%d %d %d\n", idExec, idLeg, idJud);
	//int pidsLen = strlen(pids);

	//pfd = open(EXEC_PIPE_NAME, O_WRONLY);
	//write(pfd, pids, pidsLen);
	//close(pfd);

	//pfd = open(LEGIS_PIPE_NAME, O_WRONLY);
	//write(pfd, pids, pidsLen);
	//close(pfd);
	
	//pfd = open(JUD_PIPE_NAME, O_WRONLY);
	//write(pfd, pids, pidsLen);
	//close(pfd);

	// Wait for children to set the signal handler for 
	// the passing of days
	//sem_wait(syncSem);
	//sem_wait(syncSem);
	//sem_wait(syncSem);
	//sem_close(syncSem);

	// Here starts the press work
	// Buffer to read from pipe
	//char buf[PIPE_BUF];

	// Pipe file descriptor
	//pfd = open(PRESS_NAME, O_RDONLY);

	while(day < daysLen){
		day++;

		// Reads from pipe and signals semaphore
		// to inform writer that his input has been read
		sem_wait(syncSem);
		int nBytes = read(pfd, buf, sizeof(buf));
		sem_post(syncSem2);
		if(nBytes == 0){
			fprintf(stderr, "All writers have closed their pipes\n");
			break;
		}

		// Publish headline
		printf("Dia %d: %s\n", day, ta.headline);

		// Send signal to processes informing a day has passed
		//kill(idExec, SIGUSR1);
		//kill(idLeg, SIGUSR1);
		//kill(idJud, SIGUSR1);
	}
	
	// Close pipe and semaphore
	close(pfd);
	sem_close(syncSem2);

	// Wait for the other processes to terminate
	int status;
	waitpid(idExec, &status, 0);
	waitpid(idLeg, &status, 0);
	waitpid(idJud, &status, 0);

	// Unlink semaphore and delete pipes
	sem_unlink(PRESS_SYNC_SEM2);
	char rmCom[100];
	strcpy(rmCom, "rm ");

	strcpy(rmCom + 3, PRESS_NAME);
	system(rmCom);	
	strcpy(rmCom + 3, EXEC_PIPE_NAME);
	system(rmCom);
	strcpy(rmCom + 3, LEGIS_PIPE_NAME);
	system(rmCom);
	strcpy(rmCom + 3, JUD_PIPE_NAME);
	system(rmCom);

	strcpy(rmCom + 3, EXEC_LEG_PIPE);
	system(rmCom);	
	strcpy(rmCom + 3, EXEC_JUD_PIPE);
	system(rmCom);
	
	strcpy(rmCom + 3, LEG_EXEC_PIPE);
	system(rmCom);
	strcpy(rmCom + 3, LEG_JUD_PIPE);
	system(rmCom);
	
	strcpy(rmCom + 3, JUD_EXEC_PIPE);
	system(rmCom);	
	strcpy(rmCom + 3, JUD_LEG_PIPE);
	system(rmCom);


	return 0;
}