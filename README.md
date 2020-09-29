# README

## Introduction and Features
Our shell program presents a simplified Unix command line interface which allows users to enter commands to control their computer. Specifically, our shell program is able to handle any commands used in Unix-like operation systems, including I/O redirection and pipes.

Below are some test cases that we used which you can use in our shell program:
```
$ echo "Hello World"
$ ls > out.txt
$ ls >> out2.txt
$ cat out2.txt | grep txt
$ ls | wc > out3.txt
$ exit
```

## Setup
To execute our shell program, you first need to open a terminal which is a program that grants you accessibility to shell. To compile and starts our shell program, just enter the following commands in your terminal:
```
$ make
$ ./shell
```
Press enter and you should be able to see the command line interface of our shell program. Now you can start testing our shell by entering any Unix OS commands. To shut down our the command line interface, simply enter “exit”.

**Note**: use of up arrow on keyboard to load the previous entered commands does NOT works in our shell program.

## Bugs and Known issues
None

