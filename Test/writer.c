#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>
#include <semaphore.h>

int main(){
	int pid = getpid();
	char buf[100];
	sem_t *syn = sem_open("mysem", 0);

	sprintf(buf, "Hola del proceso %d\n", pid);
	
	int n = strlen(buf);
	int fd = open("/tmp/myfifo", O_WRONLY);

	for(int i = 0; i < 10; i++){
		
		flock(fd, LOCK_EX);
		write(fd, buf, n);
		sem_wait(syn);
		flock(fd, LOCK_UN);
		
	}
	close(fd);

	return 0;
}