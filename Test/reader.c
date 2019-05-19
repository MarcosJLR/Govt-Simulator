#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

int main(){
	char *myfifo = "/tmp/myfifo";
	mkfifo(myfifo, 0666);
	sem_unlink("mysem");
	sem_t *syn = sem_open("mysem", O_CREAT, 0666, 0);
	if(syn == NULL) return 0;

	for(int i = 0; i < 10; i++){
		int pid = getpid();
		printf("%d %d\n", i, pid);
		char *argv[] = {"writer.o", NULL};
		if(fork() == 0)
			execvp("./writer.o", argv);
	}

	
	char buf[100];
	int cnt = 100;
	int fd = open(myfifo, O_RDONLY);

	while(1){
		printf("%d\n",cnt--);
		
		int nrb = read(fd, buf, sizeof(buf));
		sem_post(syn);
		if(nrb == 0) break;
		printf("%s\n",buf);

	}
	close(fd);
	for(int i = 0; i < 10; i++)
		wait(NULL);
	sem_close(syn);
	sem_unlink("mysem");
	system("rm /tmp/myfifo");

	return 0;
}