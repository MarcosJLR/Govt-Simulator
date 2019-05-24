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
int readAction(const char *filePath, char action[MAX_ACTION][MAX_ACT_LINE]);

// Carries out the action in action with nLines lines
// Returns 1 if the action was succesful and 0 if not
//int execAction(int nLines, char **action, char *dir, pid_t idExec, pid_t idLeg, pid_t idJud);

// Stores the first word of s on head and the rest on tail
void cutString(const char *s, char *head, char *tail);

// Returns 1 if line s was found on file f
int readFromFile(FILE *f, char *s);

// Writes s into file f
int writeToFile(FILE *f, char *s);

// Release the locks on file and close it, and then open the file 
// with name path with locks on mode (0 inclusive)
// If closeOnly is set, it won't open a new file
void openGovtFile(FILE **file, const char *path, int mode, int closeOnly);

// Requests aproval from proces with PID = aproverID and returns the answer
int aprovalFrom(const char *pipeName, pid_t aproverID, int sig);

#endif