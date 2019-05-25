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
#include <time.h>
#include <pthread.h> 

#include "prensaThread.h"
#include "rwoperThread.h"

int execActionE(int nLines, char action[MAX_ACTION][MAX_ACT_LINE], char dir[PATH_MAX], ContactInfo *c){
	FILE *fp = NULL;
	int success = 1;
	char com[20], inst[MAX_ACT_LINE], fileName[PATH_MAX];

	for(int i = 1; i < nLines-2; i++){
		cutString(action[i],com,inst);
		if(strcmp(com, "exclusivo:") == 0){
			strncpy(fileName, dir, sizeof(fileName));
			strncat(fileName, inst, strlen(inst) - 1);
			openGovtFile(&fp, fileName, 1, 0);
		}
		else if(strcmp(com, "inclusivo:") == 0){
			strncpy(fileName, dir, sizeof(fileName));
			strncat(fileName, inst, strlen(inst) - 1);
			openGovtFile(&fp, fileName, 0, 0);
		}
		else if(strcmp(com, "leer:") == 0){
			if(!readFromFile(fp, inst)){
				success = 0;
				break;
			}
		}
		else if(strcmp(com, "anular:") == 0){
			if(readFromFile(fp, inst)){
				success = 0;
				break;
			}
		}
		else if(strcmp(com, "escribir:") == 0){
			if(writeToFile(fp, inst) < 0){
				success = 0;
				break;
			}
		}
		else if(strcmp(com, "aprobacion:") == 0){
			if(strcmp(inst, "Congreso\n") != 0 && 
				strcmp(inst, "Tribunal Supremo\n") != 0)
				continue;

			int p = strcmp(inst, "Congreso\n");
			SecretaryArgs *s = malloc(sizeof(SecretaryArgs));
			s->req = ( p ? &(c->judReq) : &(c->legReq) );
			s->ans = ( p ? &(c->judAns) : &(c->legAns) );
			s->reqCV = ( p ? c->judReqCV : c->legReqCV );
			s->ansCV = ( p ? c->judAnsCV : c->legAnsCV );
			s->mt = ( p ? c->judMt : c->legMt );

			p = aprovalRequest(s);
			free(s);
			if(!p){
				success = 0;
				break;
			}
		}
		else if(strcmp(com, "reprobacion:") == 0){
			if(strcmp(inst, "Congreso\n") != 0 && 
				strcmp(inst, "Tribunal Supremo\n") != 0)
				continue;

			int p = strcmp(inst, "Congreso\n");
			SecretaryArgs *s = malloc(sizeof(SecretaryArgs));
			s->req = ( p ? &(c->judReq) : &(c->legReq) );
			s->ans = ( p ? &(c->judAns) : &(c->legAns) );
			s->reqCV = ( p ? c->judReqCV : c->legReqCV );
			s->ansCV = ( p ? c->judAnsCV : c->legAnsCV );
			s->mt = ( p ? c->judMt : c->legMt );

			p = aprovalRequest(s);
			free(s);
			if(p){
				success = 0;
				break;
			}
		}
	}

	openGovtFile(&fp, NULL, 0, 1);

	return success;
}

void *execThread(void *arg){
	ThreadArguments *ta = arg;

	pthread_t secID;

	SecretaryArgs *s = malloc(sizeof(SecretaryArgs));
	s->req = &(ta->c->execReq);
	s->ans = &(ta->c->execAns);
	s->reqCV = ta->c->execReqCV;
	s->ansCV = ta->c->execAnsCV;
	s->mt = ta->c->execMt;

	pthread_create(&secID, NULL, secretary, s);

	char planPath[PATH_MAX];
	char action[MAX_ACTION][MAX_ACT_LINE];

	// Path of Executive govt plan
	strncpy(planPath, ta->dir, sizeof(planPath));
	strcat(planPath, "Ejecutivo.acc");

	while(ta->end){
		int nLines = readAction(planPath, action);
		char msg[MAX_ACT_LINE];
		if(nLines == 0){
			// Ninguna accion fue escogida
			sprintf(msg, EXEC_IDDLE_MSG);
		}
		else if(nLines < 3){
			fprintf(stderr, "The Executive Govt Plan has been corrupted\n");
			break;
		}
		else{
			int success = execActionE(nLines, action, ta->dir, ta->c);
			
			if(random() % 100 >= 66) 
				success = 0;

			if(success)
				strncpy(msg, action[nLines-2] + 7, sizeof(msg));
			else
				strncpy(msg, action[nLines-1] + 9, sizeof(msg));
		}
		writeToPress(ta, msg);
	}
	return 0;

}