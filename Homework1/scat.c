//Scat.c
//Problem 1.1 a) and c)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sendfile.h>

int main(int argc, char* argv[]){
	char buffer, c, option = 'l', intermediate = getopt(argc, argv,"slp" );
	while(intermediate != -1){
		option = intermediate;
		intermediate = getopt(argc, argv,"slp" );		
	}
	off_t *offset = NULL;
	switch(option) {
			case 's':
				while(1){
					if(read(0, &buffer, 1) != 1) //read one character at a time;
						return EXIT_FAILURE;
					if(write(1, &buffer, 1) != 1)
						return EXIT_FAILURE;
					if(fflush(stdout) == EOF)
						return EXIT_FAILURE;
					
				}
				break;
			case 'p':
				while((sendfile(1,0,offset, 4096) == 4096)){
				}
				return EXIT_FAILURE;
				break;
			case 'l':
				while(1){
					c = getc(stdin);
					if(c==EOF)
						return EXIT_FAILURE;
					if(putc(c, stdout) == EOF)
						return EXIT_FAILURE;	
				}
				if(fflush(stdout) == EOF)
						return EXIT_FAILURE;
				break;
			default:
				//char c;
				while(1){
					c = getc(stdin);
					if(c==EOF)
						return EXIT_FAILURE;
					if(putc(c, stdout) == EOF)
						return EXIT_FAILURE;
					
				}
				if(fflush(stdout) == EOF)
						return EXIT_FAILURE;
				break;  
		}

	return EXIT_SUCCESS;

}