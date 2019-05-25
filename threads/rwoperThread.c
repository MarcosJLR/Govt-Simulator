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

#include "prensa.h"
#include "rwoper.h"

int writeToPress(void *arg, char *msg, int nBytes){
	struct ThreadArguments *ta = arg;
	if(nBytes >= 500){
		fprintf(stderr, "Headline is too long!\n");
		return 0;
	}

	sem_wait(&ta->write);
	sem_wait(&ta->full);
	strcpy(ta->headline[ta->head], msg);
	ta->head=(ta->head+1)%10;
	sem_post(&ta->press);
	sem_post(&ta->write);

	return wBytes;
}

int readAction(const char *filePath, char **action){
	FILE *file = fopen(filePath, "r");

	if(file == NULL){

		fprintf(stderr, "Couldn't open file: %s\n", filePath);
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

/*const char * getAction(char *dir, int prob){
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
}*/

int writeToFile(FILE *f, char *s){
	int i= fprintf(f,s);
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
		head[i] = s[i];
	}
	head[i] = '\0';
	strcpy(tail, s + i + 1);
}

void openGovtFile(FILE **file, const char *path, int mode, int closeOnly){
	if(*file){
		int fd = fileno(*file);
		flock(fd, LOCK_UN);
		fclose(*file);
		*file = NULL;
	}
	if(!closeOnly){
		*file = fopen(path, "ra");
		int fd = fileno(*file);
		if(mode)
			flock(fd, LOCK_EX);
		else
			flock(fd, LOCK_SH);
	}
}

int aprovalFrom(const char *pipeName, pid_t apID){
	char ans[2];
	kill(apID, SIGUSR2);
	int pfd = open(pipeName, O_RDONLY);
	read(pfd, ans, 1);
	close(pfd);
	return (ans[0] == 'Y');
}

int execAction(int nLines, char **action, char *dir, pid_t idExec, pid_t idLeg, pid_t idJud){
	FILE *fp;
	char com[10], inst[MAX_ACT_LINE], direction[PATH_MAX];
	for(int i = 1; i < nLines-2; i++){
		cutString(action[i],com,inst);
		if(strcmp(com, "exclusivo:") == 0){
			if(strcmp(dir, "\0")==0){
				strcat(direction, inst);
			}
			else{
				strcpy(direction, dir);
				strcat(direction, "/");
				strcat(direction, inst);
			}
			openGovtFile(&fp, direction, 1, 0);
		}
		else if(strcmp(com, "inclusivo:") == 0){
			if(strcmp(dir, "\0")==0){
				strcat(direction, inst);
			}
			else{
				strcpy(direction, dir);
				strcat(direction, "/");
				strcat(direction, inst);
			}
			openGovtFile(&fp, direction, 0, 0);
		}
		else if(strcmp(com, "leer:") == 0){
			int p = readFromFile(fp, inst);
			if(!p) return 0;
		}
		else if(strcmp(com, "anular:") == 0){
			int p = readFromFile(fp, inst);
			if(p) return 0;
		}
		else if(strcmp(com, "escribir:") == 0){
			int p = writeToFile(fp, inst);
		}
		else if(strcmp(com, "aprobación:") == 0){
			pid_t aproverID;

			if(strcmp(inst, "Congreso\n") == 0)
				aproverID = idLeg;
			else if(strcmp(inst, "Tribunal Supremo\n") == 0)
				aproverID = idJud;
			else
				aproverID = idExec;
			
			pid_t myPID = getpid();
			if(aproverID != myPID){
				char pipeName[PATH_MAX];
				if(myPID == idExec){
					if(aproverID == idLeg)
						strcpy(pipeName, LEG_EXEC_PIPE);
					else
						strcpy(pipeName, JUD_EXEC_PIPE);
				}
				else if(myPID == idJud){
					if(aproverID == idLeg)
						strcpy(pipeName, LEG_JUD_PIPE);
					else
						strcpy(pipeName, EXEC_JUD_PIPE);
				}
				else if(myPID == idLeg){
					if(aproverID == idJud)
						strcpy(pipeName, JUD_LEG_PIPE);
					else
						strcpy(pipeName, EXEC_LEG_PIPE);
				}

				if(!aprovalFrom(pipeName, aproverID))
					return 0;
			}
		}
		else if(strcmp(com, "reprobación:") == 0){
			pid_t aproverID;

			if(strcmp(inst, "Congreso\n") == 0)
				aproverID = idLeg;
			else if(strcmp(inst, "Tribunal Supremo\n") == 0)
				aproverID = idJud;
			else
				aproverID = idExec;
			
			pid_t myPID = getpid();
			if(aproverID != myPID){
				char pipeName[PATH_MAX];
				if(myPID == idExec){
					if(aproverID == idLeg)
						strcpy(pipeName, LEG_EXEC_PIPE);
					else
						strcpy(pipeName, JUD_EXEC_PIPE);
				}
				else if(myPID == idJud){
					if(aproverID == idLeg)
						strcpy(pipeName, LEG_JUD_PIPE);
					else
						strcpy(pipeName, EXEC_JUD_PIPE);
				}
				else if(myPID == idLeg){
					if(aproverID == idJud)
						strcpy(pipeName, JUD_LEG_PIPE);
					else
						strcpy(pipeName, EXEC_LEG_PIPE);
				}

				if(aprovalFrom(pipeName, aproverID))
					return 0;
			}
		}
	}
	return 1;
}
