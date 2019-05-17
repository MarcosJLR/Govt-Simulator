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

#include "judi.h"
#include "rwoper.h"

int main(int argc, char **argv){
	int day=0;
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
	while(1){

	}
}