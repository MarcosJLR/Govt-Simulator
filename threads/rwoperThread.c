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
#include <pthread.h> 

#include "rwoperThread.h"
#include "prensaThread.h"

void writeToPress(void *arg, char *msg){
	ThreadArguments *ta = arg;

	sem_wait(&ta->write);
	sem_wait(&ta->full);
	
	strcpy(ta->headline[ta->head], msg);
	ta->head = (ta->head + 1) % 10;

	sem_post(&ta->press);
	sem_post(&ta->write);
}

int readAction(const char *filePath, char action[MAX_ACTION][MAX_ACT_LINE]){
	FILE *file = fopen(filePath, "r");

	if(file == NULL){
		fprintf(stderr, "Couldn't open file: %s\n", filePath);
		return 0;
	}

	int n = 0;
	int fd = fileno(file);
	char *line = malloc(MAX_ACT_LINE*sizeof(char));
	size_t len = MAX_ACT_LINE;

	flock(fd, LOCK_SH);
	while(getline(&line, &len, file) != -1){
		int r = random() % 100;
		if(r < 10){
			strncpy(action[n++], line, sizeof(action[0]) - 1);
			while(getline(&line, &len, file) != -1){
				if(strcmp(line, "\n") == 0) break;
				strncpy(action[n++], line, sizeof(action[0]) - 1);
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

int writeToFile(FILE *f, char *s){
	int i = fprintf(f,s);
	return i;
}

int readFromFile(FILE *f, char *s){
	int n = 0;
	char *line = NULL;
	size_t len = 0;

	while(getline(&line, &len, f) != -1){
		if(strcmp(line, s) == 0){
			return 1;
		}
	}
	return 0;
}

void cutString(const char *s, char *head, char *tail){
	int i, n = strlen(s);
	for(i = 0; i < n; i++){ 		
		if(s[i] == ' ')
			break;
		if(s[i] == '\n'){
			head[i] = '\0';
			strcpy(tail, "\n");
			return;
		}
		head[i] = s[i];
	}
	head[i] = '\0';
	strcpy(tail, s + i + 1);
}

void openGovtFile(FILE **file, const char *path, int mode, int closeOnly){
	if(*file != NULL){
		int fd = fileno(*file);
		flock(fd, LOCK_UN);
		fclose(*file);
		*file = NULL;
	}
	if(!closeOnly){
		*file = fopen(path, "a+");
		int fd = fileno(*file);
		if(mode)
			flock(fd, LOCK_EX);
		else
			flock(fd, LOCK_SH);
	}
}

void *secretary(void *args){
	SecretaryArgs *realArgs = args;

	while(1){
		pthread_mutex_lock(realArgs->mt);
		while(*(realArgs->req) == 0)
			pthread_cond_wait(realArgs->reqCV, realArgs->mt);
		(*realArgs->req)--;
		(*realArgs->ans) = random() % 2;
		pthread_cond_broadcast(realArgs->ansCV);
		while(*(realArgs->ans) != -1)
			pthread_cond_wait(realArgs->ansCV, realArgs->mt);
		pthread_mutex_unlock(realArgs->mt);
	}

	free(args);

	pthread_exit(0);
}

int aprovalRequest(SecretaryArgs *s){
	int ret = 0;
	pthread_mutex_lock(s->mt);
	(*s->req)++;
	pthread_cond_broadcast(s->reqCV);
	while(*s->ans == -1)
		pthread_cond_wait(s->ansCV, s->mt);
	ret = *s->ans;
	*s->ans = -1;
	pthread_cond_broadcast(s->ansCV);
	pthread_mutex_unlock(s->mt);
	return ret;
}