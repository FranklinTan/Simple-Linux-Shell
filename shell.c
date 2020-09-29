#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h>

#include "util.h"


void pwd(){
	char cwd[256];
	printf("[4061-shell]%s $", getcwd(cwd, 256));
}

int cmd_l_input(char * input){//get input
	int temp = 0;
	char i[MAX_CMD_LEN];
	char c;

	while(1){
		c = getchar();
		if (c == EOF || c == '\n'){//end of input or enter
			i[temp] = '\0';
			break;
		}
		else{
			i[temp] = c;
		}
		temp++;
	}

	if(strlen(i) == 0){//empty input
		return 1;
	}

	strcpy(input, i);
	return 0;
}

//execute basic command
void execute(char **input_args){
	//create child process
	if (strcmp(input_args[0],"exit") == 0){
		exit(0);
	}

	if (strcmp(input_args[0],"cd") == 0){//chdir system call for cd
		if (chdir(input_args[1]) == 0){
			return;	
		}
	}

	pid_t pid = fork();
	//if succeessfully created
	if(pid == 0){
		//executes
		if (strcmp(input_args[0],"ls") == 0){
			if(execvp("./ls", input_args) == -1){// call user defined ls
				perror("Command error ");
			}
			return;
		}
		if (strcmp(input_args[0],"wc") == 0){
			if(execvp("./wc", input_args) == -1){// call user defined wc
				perror("Command error ");
			}
			return;
		}
		if(execvp(input_args[0], input_args) == -1){// other commands
			perror("Command error ");
		}
		exit(0);
		
	}else if(pid == -1){
		perror("Failed to fork child\n");
	}else{
		//parent wait for child to terminate
		wait(NULL);
	}return;

}

void execute_piped(char **before_pipe, char **after_pipe){// execute pipe
	int fd[2];
	if(pipe(fd) == -1){
		perror("Failed to pipe\n");
		return;
	}

	pid_t p1 = fork();
	if(p1 == -1){
		perror("Failed to fork child\n");
		return;
	}else if(p1 == 0){
		//first child process
		//close read end
		close(fd[0]);
		//file descriptor:  STDOUT_FILENO: 1, STDERR_FILENO: 2, STDIN_FILENO: 0
		//redirection to STDOUT_FILENO
		dup2(fd[1], STDOUT_FILENO);
		if(execvp(before_pipe[0], before_pipe) == -1){
			perror("Command error\n");
			return;
		}
	}else{
		//create second child process
		pid_t p2 = fork();
		if(p2 == -1){
			perror("Failed to fork child\n");
			return;
		}else if(p2 == 0){
			//second child
			//close write end
			close(fd[1]);
			//redirect to STDIN_FILENO
			dup2(fd[0], STDIN_FILENO);
    		//execute
    		if(execvp(after_pipe[0], after_pipe) == -1){
    			perror("Command error\n");
				return;
    		}
		}else{	
			//close read end
    		close(fd[0]);
			//close write end
			close(fd[1]);
			//parent wait for first child
			wait(NULL);
			//wait for second child
			wait(NULL);
		}
	}
}

void exec_redir(char **input_args, int flag){
	pid_t pid = fork();
	//if succeessfully created
	if(pid == 0){
		int fd;
		//executes
		if (flag == 1){
			fd = open(trimwhitespace(input_args[1]), O_RDWR | O_CREAT | O_TRUNC, 0777);// open for read and write, overwrite flag, clear before write
		}
		else{
			fd = open(trimwhitespace(input_args[1]), O_RDWR | O_APPEND | O_CREAT, 0777);// open for read and write, append flag
		}

		char *temp[MAX_TOKENS];
		parse_line(input_args[0], temp, " ");
		//get commands before the redirection

		if (fd < 0){
			perror("File error");
			exit(0);
		}
		
		dup2(fd, STDOUT_FILENO);// redirection system call
		if (dup2(fd, STDOUT_FILENO) < 0) {
			close(fd);
			perror("cannot redirect stdout");
			exit(0);
		} 

		if(execvp(temp[0], temp) == -1){
			perror("Command error ");
		}
		close(fd);
		exit(0);
		
	}else if(pid == -1){
		perror("Failed to fork child\n");
	}else{
		//parent wait for child to terminate
		wait(NULL);
	}return;
}

void exec_both(char **input_args, int flag){// execute pipe and redirection
pid_t pid = fork();
	//if succeessfully created
	if(pid == 0){
		int fd;
		//executes
		if (flag == 1){
			fd = open(trimwhitespace(input_args[1]), O_RDWR | O_CREAT | O_TRUNC, 0777);
		}
		else{
			fd = open(trimwhitespace(input_args[1]), O_RDWR | O_APPEND | O_CREAT, 0777);
		}

		if (fd < 0){
			perror("File error");
			exit(0);
		}
		
		dup2(fd, STDOUT_FILENO);
		if (dup2(fd, STDOUT_FILENO) < 0) {
			close(fd);
			perror("cannot redirect stdout");
			exit(0);
		} 

		char *parsed_pipe_input[2];
		char *front_input_args[MAX_TOKENS];
		char *back_input_args[MAX_TOKENS];

		parse_line(input_args[0], parsed_pipe_input, "|");
		parse_line(parsed_pipe_input[0], front_input_args, " ");
    	parse_line(parsed_pipe_input[1], back_input_args, " ");
		//execute 
        execute_piped(front_input_args, back_input_args);

		close(fd);
		exit(0);
		
	}else if(pid == -1){
		perror("Failed to fork child\n");
	}else{
		//parent wait for child to terminate
		wait(NULL);
	}return;
}

int check_pipe(char *input){// check all the input for '|'
	for (int i = 0; i < strlen(input); i++){
		if (input[i] == '|'){
			return 0;
		}
	}
	return 1;
}

int check_redirection(char *input){// check all the input for '>' or '>>'
	int num = 0;
	char pre_char = 'a';
	for (int i = 0; i < strlen(input); i++){
		if (input[i] == '>'){
			if (pre_char == '>'){
				num = 2;
				return num;
			}
			num = 1;
		}
		pre_char = input[i];
	}
	return num;
}

int main(int argv, char *args[]){
	//command line input 
	char input[MAX_CMD_LEN];
	char *input_args[MAX_TOKENS];
	int num_of_token;

	while(1){
		//init shell and print current working directory
		pwd();
		//check input 
		if(cmd_l_input(input) != 0){
			continue;
		}
        //if command line input is piped and redirction
        if(check_redirection(input) > 0 && check_pipe(input) == 0){
			if (check_redirection(input) == 1){	
				parse_line(input, input_args, ">");
				exec_both(input_args, 1);
			}

			else if (check_redirection(input) == 2){
				parse_line(input, input_args, ">");
				exec_both(input_args, 2);
			}

        }
		//if command line input is redirction
		else if (check_redirection(input) > 0){
			if (check_redirection(input) == 1){	
				//printf("redirction here, overwrite\n");
				parse_line(input, input_args, ">");
				exec_redir(input_args, 1);
			}

			else if (check_redirection(input) == 2){
				//printf("redirction here, append\n");
				parse_line(input, input_args, ">>");
				exec_redir(input_args, 2);
			}
		}

		//if command line input is piped
		else if (check_pipe(input) == 0){
			
			char *parsed_pipe_input[2];
			char *front_input_args[MAX_TOKENS];
			char *back_input_args[MAX_TOKENS];

			// parse command before and after | separately
			parse_line(input, parsed_pipe_input, "|"); 
			// process 2 commands (before and after pipe)
			parse_line(parsed_pipe_input[0], front_input_args, " ");
        	parse_line(parsed_pipe_input[1], back_input_args, " ");
			//execute 
         	execute_piped(front_input_args, back_input_args);
		}
		// other commands
		else{
        	//parses command line input 
		    //parsed line stored into input_args
        	parse_line(input, input_args, " ");
        	execute(input_args);
        }

	}
	return 0;
}