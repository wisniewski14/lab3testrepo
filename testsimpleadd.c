//Student: Jesse Wisniewski
//ID: 103130867
//CSCI 3753 Operating Systems
//TA: Rahat Rafiq

#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <stdio.h>		
#include <stdlib.h>

int main(int argc, char *argv[]){
	int result;
	int *resultpointer;
	int num1;
	int num2;
	
	num1 = 5; //Default values
	num2 = 3; //
	resultpointer = &result;
	
	if(argc == 3){
		// Argument values
		num1 = atoi(argv[1]);
		num2 = atoi(argv[2]);
	}else if(argc != 1){
		// Wrong numbers of arguments
		printf("Should be two arguments, or none.\n");
		exit(0);
	}
	//System call
	syscall(319,num1,num2,resultpointer);
	printf("Adding %d and %d, result is %d\n",num1,num2,result);
}
