#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>

void main(int argc, char *argv[])
{
	struct hostent *myhost;
	struct in_addr myinaddr;
	int i;
	if(argc < 2)
	{
	printf("Usage: %s host_name(domain name)\n", argv[0]);
	exit(1);
	}

	myhost=gethostbyname(argv[1]);
	if(myhost==0)
	{
	printf("error occurs .. at 'gethostbyname'.\n\n\n");
	exit(1);
	}

	printf("official host name : \t\t %s\n", myhost->h_name);
	i = 0;
	while(myhost->h_aliases[i]!=NULL)
	{
		printf("aliases name: \t\t%s\n", myhost->h_aliases[i]);
		i++;
	}

	printf("host address type: \t\t%d\n", myhost->h_addrtype);

	printf("length of host address: \t%d\n", myhost->h_length);
	i = 0;
	while(myhost->h_addr_list[i]!=NULL)
	{
		myinaddr.s_addr=*((u_long *)(myhost->h_addr_list[i]));
		printf("address for host: \t\t%s\n", inet_ntoa(myinaddr));
		i++;
	}
}
