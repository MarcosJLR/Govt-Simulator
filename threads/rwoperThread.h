#ifndef RWOPERTHREAD_H
#define RWOPERTHREAD_H

// Max number of lines in a Govt. plan action
#undef MAX_ACTION
#define MAX_ACTION 128

// Max number of char in a line of a Govt. plan
#undef MAX_ACT_LINE
#define MAX_ACT_LINE 1024

typedef struct secretaryArgs{
	int *req, *ans;
	pthread_cond_t *reqCV, *ansCV;
	pthread_mutex_t *mt;
} SecretaryArgs;

// Writes to press message msg through the buffer defined in arg
void writeToPress(void *arg, char *msg);

// Reads and selects an action from filePath Govt Plan
// and stores it in action and returns the number of lines read
int readAction(const char *filePath, char action[MAX_ACTION][MAX_ACT_LINE]);

// Writes string s into file f
int writeToFile(FILE *f, char *s);

// Searches line s in file f and returns 1 if found
// and 0 otherwise
int readFromFile(FILE *f, char *s);

// Stores the first word of s in head and the rest in tail
void cutString(const char *s, char *head, char *tail);

// Release the locks on file and close it, and then open the file 
// with name path with locks on mode (0 inclusive)
// If closeOnly is set, it won't open a new file
void openGovtFile(FILE **file, const char *path, int mode, int closeOnly);

// Secretary to attend to some threads aproval Requests
void *secretary(void *args);

// Request aproval from secretary with secretary args s
int aprovalRequest(SecretaryArgs *s);

#endif