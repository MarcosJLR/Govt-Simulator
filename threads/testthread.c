#include <pthread.h> 
#include <stdio.h>

struct ThreadArguments{
	char headline[500];
	char exec[500];
	char judi[500];
	char legis[500];
	char exec_leg[500];
	char exec_jud[500];
	char jud_leg[500];
	char jud_exec[500];
	char leg_exec[500];
	char leg_jud[500];
	char dir[500];
	int days;
};

void* execThread(void *ta){
	struct ThreadArguments *arg = ta;
	strcpy(arg->exec, "Epa.");
	int i=0;
	while(i<20){
		printf("%d.Epa, es hilo.\n",i);	
		i++;
	}
}

int main(int argc, char **argv){
	int daysLen, day = 0;
	pthread_t exec, legis, judi;
	char dir[500];
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
	pthread_create(&exec, NULL, execThread, &ta);
	int i=0;
	while(i<100){
		printf("%d.Epa, es principal.\n",i);	
		i++;
	}
	pthread_kill(exec, SIGUSR1);
	printf("%s \n",ta.exec);
}