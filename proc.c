
int main(int argc, char **argv){
	int id_exec = fork();
	if(id_exec==0){
		//Ejecutivo
	}
	else{
		int id_leg = fork();
		if(id_leg==0){
			//Legislativo
		}
		else{
			int id_jud = fork();
			if(id_jud==0){
				//Judicial
			}
			else{
				//Press
			}			
		}
	}
}