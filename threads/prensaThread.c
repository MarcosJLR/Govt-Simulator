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

#include "rwoperThread.h"
#include "prensaThread.h"
#include "execThread.h"
#include "legisThread.h"
#include "judiThread.h"


int main(int argc, char **argv){

	char dir[PATH_MAX];
	int daysLen, day = 0;
	pthread_t exec, legis, judi;
	
	// Needs to have at least the duration of the simulation
	if(argc < 2){
		fprintf(stderr, "Too few arguments\n");
		return 0;
	}
	
	// Number of days the simulation will run
	sscanf(argv[1], "%d", &daysLen);
	
	// Path of directory where Govt. files exist
	if(argc > 2)
		strncpy(dir, argv[2], sizeof(dir));

	// Creating a struct with arguments pssed to thread
	ThreadArguments ta;
	strncpy(ta.dir, dir, sizeof(ta.dir));
	ta.days = daysLen;
	ta.head = 0;
	ta.end = 1;
	sem_init(&ta.write, 0, 1);
	sem_init(&ta.press, 0, 0);
	sem_init(&ta.full, 0, 10);

	ta.c = malloc(sizeof(ContactInfo));

	ta.c->execReq = ta.c->legReq = ta.c->judReq = 0;
	ta.c->execAns = ta.c->legAns = ta.c->judAns = -1;
	
	ta.c->execMt = malloc(sizeof(pthread_mutex_t));
	ta.c->legMt = malloc(sizeof(pthread_mutex_t));
	ta.c->judMt = malloc(sizeof(pthread_mutex_t));

	ta.c->execReqCV = malloc(sizeof(pthread_cond_t));
	ta.c->legReqCV = malloc(sizeof(pthread_cond_t));
	ta.c->judReqCV = malloc(sizeof(pthread_cond_t));

	ta.c->execAnsCV = malloc(sizeof(pthread_cond_t));
	ta.c->legAnsCV = malloc(sizeof(pthread_cond_t));
	ta.c->judAnsCV = malloc(sizeof(pthread_cond_t));

	// Init Executive
	pthread_create(&exec, NULL, execThread, &ta); 

	// Init Legislative
	pthread_create(&legis, NULL, legisThread, &ta);

	// Init Judicial
	pthread_create(&judi, NULL, judiThread, &ta);


	int j = 0;
	while(day < daysLen){
		day++;

		// Wait for someone to write in the buffer
		sem_wait(&ta.press);
		
		// Publish headline
		printf("Dia %d: %s\n", day, ta.headline[j]);
		j = (j + 1) % 10;

		sem_post(&ta.full);
	}
	ta.end = 0;
	// Close pipe and semaphore
	sem_destroy(&ta.press);
	sem_destroy(&ta.write);
	sem_destroy(&ta.full);

	// Wait for the other processes to terminate

	return 0;
}