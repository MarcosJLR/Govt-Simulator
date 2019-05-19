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
#include <sys/file.h>
#include <time.h>

#include "prensa.h"
#include "rwoper.h"

int writeToPress(int fd, char *msg, int nBytes, sem_t *syncSem){
	if(nBytes >= PIPE_BUF){
		fprintf(stderr, "Headline is too long!\n");
		return 0;
	}

	flock(fd, LOCK_EX);
	int wBytes = write(fd, msg, nBytes);
	sem_wait(syncSem);
	flock(fd, LOCK_UN);

	return wBytes;
}

int readAction(const char *filePath, char **action){
	FILE *file = fopen(filePath, "r");

	if(file == NULL){
		fprintf(stderr, "Couldn't open file\n");
		return 0;
	}

	int n = 0;
	int fd = fileno(file);
	char *line = NULL;
	size_t len = 0;
	srandom(time(NULL));

	flock(fd, LOCK_SH);
	while(getline(&line, &len, file) != -1){
		int r = random() % 100;
		if(r < 20){
			strcpy(action[n++], line);
			while(getline(&line, &len, file) != -1){
				if(strcmp(line, "\n") == 0) break;
				strcpy(action[n++], line);
			}
		}
		else{
			while(getline(&line, &len, file) != -1)
				if(strcmp(line, "\n") == 0) break;
		}
		if(n) break;
	}
	flock(fd, LOCK_UN);
	free(line);
	fclose(file);

	return n;
}

const char * getAction(char *dir, int prob){
	FILE *fp
	fp= fopen(*dir, r);
	char act[10000];
	char ch, chp;
	int accepted=0;
	act[0]='\0'
	int charnum=0;
	if((rand()%100)<prob){
		accepted=1;
	}
	chp='\0';
	while((ch = fgetc(fp)) != EOF){
		if(accepted){
			if(ch=='\n' && chp=='\n'){
				fclose(fp);
				return act;
			}
			else{
				act[charnum]=ch;
				charnum++;
			}
		}
		else{
			if(ch=='\n' && chp=='\n'){
				if((rand()%100)<prob){
					accepted=1;
				}
			}
		}
		chp=ch;
	}
	fclose(fp);
	return act;
}

int execAction(const int nLines, const char **action){

}