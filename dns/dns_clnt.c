#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
	char buff_snd[BUFSIZE+5];
	char buff_rcv[BUFSIZE+5];
	int addr_size;
	struct sockaddr_in serv_addr;
	struct sockaddr_in from_addr;

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_DGRAM, 0);
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));

	while(1) 
	{
		fputs("Input Domain or IP(Q to quit): ", stdout);
		fgets(buff_snd, sizeof(buff_snd), stdin);
		
		if(!strcmp(buff_snd,"q\n"))
			break;
			
		sendto(sock, buff_snd, strlen(buff_snd)+1, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
		addr_size=sizeof(from_addr);
		recvfrom(sock, buff_rcv, BUFSIZE, 0, (struct sockaddr*)&from_addr, &addr_size);
		
		printf("message from server: %s\n", buff_rcv);
	}
		
		
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
