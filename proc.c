#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



int main(int argc, char **argv){
	if(argc<2) return 0;
	int days_len, day;
	sscanf(argv[1], "%d", &days_len);
	char dir[100];
	dir="";
	if(argc>2){
		dir=argv[2];
	}
	day=0;
	int id_exec = fork();
	if(id_exec==0){
		//Ejecutivo
		while(day<days_len){

		}
	}
	else{
		int id_leg = fork();
		if(id_leg==0){
			//Legislativo
			while(day<days_len){
					
			}
		}
		else{
			int id_jud = fork();
			if(id_jud==0){
				//Judicial
				while(day<days_len){
					
				}
			}
			else{
				//Press
				while(day<days_len){
					
				}
			}			
		}
	}
}