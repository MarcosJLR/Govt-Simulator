#ifndef PRENSA_H
#define PRENSA_H

#undef PRESS_NAME
#define PRESS_NAME "/tmp/GovtPress"

#undef PRESS_SYNC_SEM
#define PRESS_SYNC_SEM "/tmp/PressSyncSem"

#undef EXEC_PIPE_NAME
#define EXEC_PIPE_NAME "/tmp/PressExecutive"

#undef LEGIS_PIPE_NAME
#define LEGIS_PIPE_NAME "/tmp/PressLegislative"

#undef JUD_PIPE_NAME
#define JUD_PIPE_NAME "/tmp/PressJudicial"

#undef EXEC_IDDLE_MSG
#define EXEC_IDDLE_MSG "El Presidente se tomo un receso de un dia"

#undef LEGIS_IDDLE_MSG
#define LEGIS_IDDLE_MSG "El Congreso se tomo un receso de un dia"

#undef JUD_IDDLE_MSG
#define JUD_IDDLE_MSG "El Tribunal se tomo un receso de un dia"

#endif