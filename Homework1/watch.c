//watch.c
//Problem 1.2 a) and c)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char* argv[]){
	unsigned int sleepTime = 2;
	int flagForB = 0, FlagForE = 0, status, option, i;
	
	while((option = getopt(argc, argv,"n:be")) != -1){
		switch(option){
			case 'n':
				sleepTime = atoi(optarg);
				continue;
			
			case 'e':
				FlagForE = 1;
				continue;

			case 'b':
				flagForB = 1;
				continue;
			case '?':
				//perror("Argument requires a value \n");
				exit(EXIT_FAILURE);
		}
	}
	pid_t pids;
	char *execute[argc - optind +1];
	for (i = optind; i < argc; i++)
			execute[i - optind] = argv[i];

	execute[i] = '\0';
	

	
	while(1){
		pids = fork();
		if(pids == 0){
			//it is a child process
			if(execvp(execute[0], execute)!= 0){
				if(flagForB){
					printf("\a\n");
					exit(EXIT_FAILURE);
				}

			}
			exit(EXIT_SUCCESS);
		}
		else if(pids < 0){
			exit(EXIT_FAILURE);

		}
		else {
			//wait for any child process 
			waitpid(-1, &status, WUNTRACED);
			if(FlagForE && status){
				exit(EXIT_FAILURE);
			}
			sleep(sleepTime);
		}

	}
	return EXIT_SUCCESS;


}