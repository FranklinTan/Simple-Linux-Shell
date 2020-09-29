#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>
#include <string.h>

void cd(char* arg){
	/*insert code here*/
	char path[256];
	//print current directory
	printf("%s\n", getcwd(path, 256));

	if (chdir(arg) == 0){
		printf("%s\n", getcwd(path, 256));
	}
	else{
		//print directory after chdir()
		printf("%s\n", "Error, cannot change directory");
	}
}

int main(int argc, char** argv){

	if(argc<2){
		printf("Pass the path as an argument\n");
		return 0;
	}
	cd(argv[1]);
	return 0;
}