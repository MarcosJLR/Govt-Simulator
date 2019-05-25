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
	char headline[10][500];
	char exec[500], judi[500], legis[500];
	char exec_leg[500], exec_jud[500];
	char jud_leg[500], jud_exec[500];
	char leg_exec[500], leg_jud[500];
	char dir[PATH_MAX];
	int days;
	pthread_t *exec, *legis, *judi;
	sem_t write;
	sem_t press;
	int head, end;
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
	ta.head=0;
	ta.end=1;
	sem_init(&ta.write,0,1);
	sem_init(&ta.press,0,0);


	// Init Executive
	pthread_create(&exec, NULL, execThread, &ta); 

	// Init Legislative
	pthread_create(&legis, NULL, legisThread, &ta);


	// Init Judicial
	pthread_create(&judi, NULL, judiThread, &ta);


	int j=0;
	while(day < daysLen){
		day++;

		// Reads from pipe and signals semaphore
		// to inform writer that his input has been read
		sem_wait(&ta.press);
		

		// Publish headline
		printf("Dia %d: %s\n", day, ta.headline[j]);
		strcpy(ta.headline[j], "\0");
		j=(j+1)%10;
		sem_post(&ta.press);
		// Send signal to processes informing a day has passed
		//kill(idExec, SIGUSR1);
		//kill(idLeg, SIGUSR1);
		//kill(idJud, SIGUSR1);
	}
	ta.end=0;
	// Close pipe and semaphore
	sem_destroy(ta.press);
	sem_destroy(ta.write);

	// Wait for the other processes to terminate

	return 0;
}