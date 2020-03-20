
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>



#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/wait.h>
#include <netdb.h>


void error(char *msg)
{
    perror(msg);
    exit(1);
}


void dostuff (int sock)
{
   int n;
   char buffer[256];
      
   bzero(buffer,256);
   n = read(sock,buffer,255);
   if (n < 0) error("ERROR reading from socket");
   printf("Here is the message: %s\n",buffer);
   n = write(sock,"I got your message",18);
   if (n < 0) error("ERROR writing to socket");
}

static int page(int fd1){
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

        int n = write(fd1,help_buffer,sizeof(help_buffer));
        if (n < 0) error("ERROR writing to socket");

        
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

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen, pid;
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     while (1) {
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) 
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             page(newsockfd);
             exit(0);
         }
         else close(newsockfd);
     } /* end of while */
     return 0; /* we never get here */
}










/*int main(int argc, char *argv[])
{
    pid_t pid;
    int sockfd, newsockfd, portno, clilent;
    //char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    //int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    //create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    //set the server socket address 
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; //INADD_ANY gives the addres of host machine
    serv_addr.sin_port = htons(portno); // convert port number in host byte order to a port number in network byte order.

    //bind the socket address to the socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0) 
            error("ERROR on binding");

    //listen to connection, setting the backlog queue upperbound to 5
    listen(sockfd,5); 
    clilent = sizeof(cli_addr);

     //accept the connection, and rear end communication address(client) is stored in cli_addr
     

    while(1){
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilent);
        if (newsockfd < 0) 
            error("ERROR on accept");


        pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        if (pid == 0)  {
            close(sockfd);
            dostuff(newsockfd);
            exit(0);
        }else{
            close(sockfd);
        }
    }





    return 0; 
}

*/