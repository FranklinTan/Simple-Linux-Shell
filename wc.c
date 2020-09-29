#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>
#include<string.h>
#include<ctype.h>
#include<stdarg.h>

void wc(int mode, char* path){
	/*Feel free to change the templates as needed*/
	/*insert codw here*/
	FILE *fptr = fopen(path, "r");
	if (fptr == NULL){
		printf("Cannot open file\n");
		return;
	}

	int char_count = 0;
	int word_count = 0;
	int line_count = 0;
	int dup = 0;
	char pre_char = 'a';//pre_char: variable to check mutiple space 
	char temp;
	while((temp = fgetc(fptr)) != EOF){
		char_count += 1;

		if(temp == '\n' || temp == '\0' || temp == ' ' || temp == '\t' || ispunct(temp) != 0){
			if (!isalpha(pre_char)){
				word_count += 1;
			}
		}

		if (temp == '\n' || temp == '\0'){
			line_count += 1;
		}		
		pre_char = temp;
	}

	if (mode == 0){
		printf(" %d %d %d %s\n", line_count, word_count, char_count, path);
	}
	
	else if (mode == 1){
		printf(" %d %s\n", line_count, path);
	}

	else if (mode == 2){
		printf(" %d %s\n", word_count, path);
	}

	else if (mode == 3){
		printf(" %d %s\n", char_count, path);
	}	
	fclose(fptr);
}

int main(int argc, char** argv){
	if(argc>2){
		if(strcmp(argv[1], "-l") == 0) { 
			wc(1, argv[2]);
		} else if(strcmp(argv[1], "-w") == 0) { 
			wc(2, argv[2]);
		} else if(strcmp(argv[1], "-c") == 0) { 
			wc(3, argv[2]);
		} else {
			printf("Invalid arguments\n");
		}
	} else if (argc==2){
	 	if(strcmp(argv[1], "-l") == 0) { 
			wc(1, NULL);
		} else if(strcmp(argv[1], "-w") == 0) { 
			wc(2, NULL);
		} else if(strcmp(argv[1], "-c") == 0) { 
			wc(3, NULL);
		} else {
    		wc(0, argv[1]);
    	}
  	} else {
  		wc(0,NULL);
  	}

	return 0;
}