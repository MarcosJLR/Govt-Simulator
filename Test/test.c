#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <time.h>

int main(){
	
	FILE *file = fopen("prueba.txt", "r");

	int n = 0;
	char *line = NULL;
	size_t len = 0;
	while(getline(&line, &len, file) != -1){
		if(n != 3) printf("%s", line);
		n++;
	}

	FILE *file2 = fopen("prueba.txt", "w");
	
	

	printf("%d %d\n", fileno(file), fileno(file2));

	return 0;	
}