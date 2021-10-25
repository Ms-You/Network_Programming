#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio_ext.h>

#define BUF_SIZE 100
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
	char message[BUF_SIZE];
	int str_len;
	int cnt[1];
	cnt[0] = 0;

	struct sockaddr_in serv_addr;

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);   

	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
		error_handling("connect() error!");
	
  	write(sock, argv[1], BUF_SIZE);
	while(1){  	        
		read(sock, message, BUF_SIZE);
		if(strcmp(message, "congraturation") == 0){
			printf("%s\n", message);
			break;
		}
		
		printf("word: %s\n", message);
		
		char letter[1];
		letter[0] = getchar();	// inpurt letter
		__fpurge(stdin);
		printf("input letter: %c\n", letter[0]);
	        write(sock, letter, 1);
		cnt[0]++;

	}
	
	write(sock, cnt, 1);
	printf("You cleared in %d times.\n", cnt[0]);
        close(sock);
        return 0;
}


void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
