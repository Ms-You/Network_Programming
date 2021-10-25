#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <time.h>

#define BUF_SIZE 100

void error_handling(char *message);
void z_handler(int sig); // 좀비 핸들러

int main(int argc, char *argv[])
{	

	char letter;	// recieve letter from client
	int cnt = 0;	// client's trial count
	char *array[] = {"apple", "banana", "cherry"};
	char *word;	// random word
	int len;	// length of random word
	int i;
	
        int fd1[2], fd2[2];
        char buffer[BUF_SIZE];

	int serv_sock, clnt_sock;
	struct sockaddr_in serv_addr, clnt_addr;
	
	pid_t pid;
	struct sigaction act;
	int str_len, state, addr_size;
	
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

        if(pipe(fd1) < 0 || pipe(fd2) < 0)
		error_handling("Pipe() error!!");

	act.sa_handler=z_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;

	state=sigaction(SIGCHLD, &act, 0);
        if (state != 0) 
            error_handling("sigaction() error");

	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");
	
        if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");
	
	while(1)
        {
		addr_size = sizeof(clnt_addr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_size);
		if(clnt_sock==-1)
		   continue;
				
                pid=fork();
		if(pid==-1)
		{
  		   close(clnt_sock);
		   continue;
		}
		else if(pid>0) /* Parent Process */
		{
		     read(clnt_sock, buffer, BUF_SIZE);
		     printf("%s Connection Created..\n", buffer);
  	             close(clnt_sock);


                     srand(time(NULL));
                     int wnum = sizeof(array)/sizeof(char *);
                     int ran_word = rand() % wnum;
                     word = array[ran_word];	// random word generated
                     printf("generated word: %s\n", word);
                     
                     write(fd1[1], word, sizeof(word));	// send random word to child process

                     read(fd2[0], buffer, BUF_SIZE);	// recieve trying count from child process
                     
                     printf("trial count: %d\n", buffer[0]);
                     // have to make log file and input buffer(trying count) and word
		    	
                 }
                 else {  /* Child Process */
                     close(serv_sock);
                     
                     read(fd1[0], buffer, BUF_SIZE);	// recieve the word from parent process
                     
                     len = strlen(buffer);
                     
                     char pword[len];
                     for(int i = 0; i < len; i++)
				pword[i] = '_';
		     
		     char *word2 = buffer;	// word to compare with pword in child process
  
                     while(1){
	                       char buffer2[BUF_SIZE];
				if(strcmp(word2, pword) == 0){
					sprintf(pword, "%s", "congraturation");
					write(clnt_sock, pword, BUF_SIZE);
					
					break;
				}
				
                              write(clnt_sock, pword, sizeof(pword));	// send the length of word to client
                     
				read(clnt_sock, buffer2, 1);	// recieve the letter from client
				
				printf("letter from client: %s\n", buffer2);
				
				for(int j = 0; j < len; j++){
					if(word2[j] == buffer2[i])
						pword[j] = buffer2[i];
				}

                     
                     }
                     char buffer3[BUF_SIZE];
                     
                     read(clnt_sock, buffer3, BUF_SIZE);	// recieve trial count
                     
                     write(fd2[1], buffer3, BUF_SIZE);	// send trial count to parent process
                     
                     puts("Disconnected..");
                     close(clnt_sock);
                     exit(0);
                }
        } 
        return 0;
}      



void z_handler(int sig)
{
	pid_t pid;
	int status;

	pid=waitpid(-1, &status, WNOHANG);
	printf("removed proc id: %d \n", pid);
        printf("Returned data : %d \n\n", WEXITSTATUS(status));
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
