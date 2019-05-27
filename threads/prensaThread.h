#ifndef PRENSATHREAD_H
#define PRENSATHREAD_H

#include "rwoperThread.h"

#undef EXEC_IDDLE_MSG
#define EXEC_IDDLE_MSG "El Presidente se tomo un receso de un dia\n"

#undef LEGIS_IDDLE_MSG
#define LEGIS_IDDLE_MSG "El Congreso se tomo un receso de un dia\n"

#undef JUD_IDDLE_MSG
#define JUD_IDDLE_MSG "El Tribunal se tomo un receso de un dia\n"

typedef struct contactInfo{
	int execReq, legReq, judReq;
	int execAns, legAns, judAns;
	pthread_cond_t *execReqCV, *legReqCV, *judReqCV; 
	pthread_cond_t *execAnsCV, *legAnsCV, *judAnsCV;
	pthread_mutex_t *execMt, *legMt, *judMt;
} ContactInfo;

typedef struct threadArguments{
	char headline[10][MAX_ACT_LINE];
	sem_t write, press, full;
	int head, end, days, stats[3];
	char dir[PATH_MAX];	
	ContactInfo *c;
} ThreadArguments;

#endif