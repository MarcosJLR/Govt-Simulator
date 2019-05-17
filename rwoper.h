#ifndef RWOPER_H
#define RWOPER_H

// Writes in the press pipe nBytes bytes from buf
// syncSem is the semaphore to wait for press to read the headline
// Returns number of bytes written to the pipe
int writeToPress(int fd, char *buf, int nBytes, sem_t *syncSem);

#endif