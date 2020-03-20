#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>

static const char *progname = "daytime";

#ifndef NI_MAXHOST
#define NI_MAXHOST	1025
#endif
#ifndef NI_MAXSERV
#define NI_MAXSERV	32
#endif




static int page(){
    ssize_t len, cnt;
    int z = 0,countspace = 0, status, pid, fd[2];
    status = pipe(fd);
    if (status == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }
    pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (pid == 0) {
        close(fd[1]);
        status = dup2(fd[0], STDIN_FILENO);
        if (status == -1) {
            perror("dup2");
            return EXIT_FAILURE;
        }
        close(fd[0]);
        char buffer[250], help_buffer[250];
        fgets(buffer, sizeof(buffer), stdin);


		int i = 0;
        while(buffer[z+1] != '.' && countspace < 15){
        	if(buffer[z] == ' ')
        		countspace++;
        	help_buffer[i] = buffer[z];
        	z++;
        	i++;
        }

        fprintf(stdout, "%s\n", help_buffer);
        
    } else {
        close(fd[0]);
        status = dup2(fd[1], STDOUT_FILENO);
        if (status == -1) {
            perror("dup2");
            return EXIT_FAILURE;
        }
        close(fd[1]);
        execl("/usr/games/fortune", "fortune", NULL);
        do {
            if (waitpid(pid, &status, 0) == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return EXIT_SUCCESS;
}
int main(){
 int a = page();
 return 0;
}
