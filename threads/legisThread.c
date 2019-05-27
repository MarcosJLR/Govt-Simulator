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

int execActionL(int nLines, char action[MAX_ACTION][MAX_ACT_LINE], char dir[PATH_MAX], ContactInfo *c){
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
			if(strcmp(inst, "Congreso\n") == 0)
				continue;

			int p = strcmp(inst, "Tribunal Supremo\n");
			SecretaryArgs *s = malloc(sizeof(SecretaryArgs));
			s->req = ( p ? &(c->execReq) : &(c->judReq) );
			s->ans = ( p ? &(c->execAns) : &(c->judAns) );
			s->reqCV = ( p ? c->execReqCV : c->judReqCV );
			s->ansCV = ( p ? c->execAnsCV : c->judAnsCV );
			s->mt = ( p ? c->execMt : c->judMt );

			p = aprovalRequest(s);
			free(s);
			if(!p){
				success = 0;
				break;
			}
		}
		else if(strcmp(com, "reprobacion:") == 0){
			if(strcmp(inst, "Congreso\n") == 0)
				continue;

			int p = strcmp(inst, "Tribunal Supremo\n");
			SecretaryArgs *s = malloc(sizeof(SecretaryArgs));
			s->req = ( p ? &(c->execReq) : &(c->judReq) );
			s->ans = ( p ? &(c->execAns) : &(c->judAns) );
			s->reqCV = ( p ? c->execReqCV : c->judReqCV );
			s->ansCV = ( p ? c->execAnsCV : c->judAnsCV );
			s->mt = ( p ? c->execMt : c->judMt );

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

void* legisThread(void *arg){
	ThreadArguments *ta = arg;

	pthread_t secID;

	SecretaryArgs *s = malloc(sizeof(SecretaryArgs));
	s->req = &(ta->c->legReq);
	s->ans = &(ta->c->legAns);
	s->reqCV = ta->c->legReqCV;
	s->ansCV = ta->c->legAnsCV;
	s->mt = ta->c->judMt;

	pthread_create(&secID, NULL, secretary, s);

	char planPath[PATH_MAX];
	char action[MAX_ACTION][MAX_ACT_LINE];

	// Path of Legislative govt plan
	strncpy(planPath, ta->dir, sizeof(planPath));
	strcat(planPath, "Legislativo.acc");

	while(ta->end){
		ta->stats[4]++;
		int nLines = readAction(planPath, action);
		char msg[MAX_ACT_LINE];
		if(nLines == 0){
			// Ninguna accion fue escogida
			sprintf(msg, LEGIS_IDDLE_MSG);
		}
		else if(nLines < 3){
			fprintf(stderr, "The Legislative Govt Plan has been corrupted\n");
			break;
		}
		else{
			int success = execActionL(nLines, action, ta->dir, ta->c);
			
			if(random() % 100 >= 66) 
				success = 0;

			if(success)
				strncpy(msg, action[nLines-2] + 7, sizeof(msg));
			else
				strncpy(msg, action[nLines-1] + 9, sizeof(msg));

			ta->stats[1] += success;
		}
		writeToPress(ta, msg);
	}
	return 0;

}