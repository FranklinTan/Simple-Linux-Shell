#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>
#include<string.h>
#include <stdbool.h>
#include <dirent.h>

void ls(char *path, bool recurse_flag) {
	/* Insert code here*/
	char p[256];
	if (path == NULL){
		getcwd(p, 256);
	}
	else{ 	
		strcpy(p, path);
	}
	DIR *dir = opendir(p);
	if (!dir){
			printf("Error, cannot open directory");
			return;
	}
	
	struct dirent *dir_path;
	while(((dir_path = readdir(dir)) != NULL)){
		if (strcmp(dir_path->d_name, ".") != 0 && strcmp(dir_path->d_name, "..") != 0) {
			char name[256];
			name[0] = '\0';
			strcpy(name, p);
			strcat(name, "/");
			strcat(name, dir_path->d_name);
			if (recurse_flag && (dir_path->d_type == DT_DIR)) {
				printf("\n>>>%s:\n", dir_path->d_name);//print directory
				ls(name, true);//recursive call
			} 
			else {
				printf("%s\n", dir_path->d_name);
			}
		}		
	}
	closedir(dir);
}

int main(int argc, char *argv[]){
	if(argc < 2){ // No -R flag and no path name
		ls(NULL, false);
	} else if(strcmp(argv[1], "-R") == 0) { 
		if(argc == 2) { // only -R flag
			ls(NULL, true);
		} else { // -R flag with some path name
			ls(argv[2], true);
		}
	}else { // no -R flag but path name is given
    	ls(argv[1], false);
  }
	return 0;
}
