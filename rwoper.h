#ifndef RWOPER_H
#define RWOPER_H

// Max number of lines in a Govt. plan action
#undef MAX_ACTION
#define MAX_ACTION 128

// Max number of char in a line of a Govt. plan
#undef MAX_ACT_LINE
#define MAX_ACT_LINE 1024

// Writes in the press pipe (fd file descriptor) nBytes bytes from msg
// syncSem is the semaphore to wait for press to read the headline
// Returns number of bytes written to the pipe
int writeToPress(int fd, char *msg, int nBytes, sem_t *syncSem);

// Extracts an action from the govt plan and saves it in act
// (at most x lines)
int readAction(const char *filePath, char **action);

// Carries out the action in action with nLines lines
// Returns 1 if the action was succesful and 0 if not
int execAction(int nLines, char **action);

#endif