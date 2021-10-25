#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>

#define BUCKET_SIZE 100 // 버켓의 총 길이
#define BUFSIZE 1024

struct bucket* hashTable = NULL;

void error_handling(char *message);

// 오류 처리 함수
void error(char *message) {
	fprintf(stderr, "%s\n", message);
	exit(1);
}


struct node {
    char *key;
    char *value;
    struct node* next;
    struct node* previous;
};


struct bucket{
    struct node* head;
    int count;
};


struct node* createNode(char *key, char *value){
    struct node* newNode;
    newNode = (struct node *)malloc(sizeof(struct node));
    
    newNode->key = key;
    newNode->value = value;
    newNode->next = NULL;
    newNode->previous = NULL;

    return newNode;
}


int hashFunction(char *key){
	int hash = 0;
	
	for(int i = 0; i < strlen(key); i++){
		hash += (int)key[i];
	}
	
    return hash%BUCKET_SIZE;
}


void add(char *key, char *value){
    
    int hashIndex = hashFunction(key);
    
    struct node* newNode = createNode(key, value);
    
    
    if (hashTable[hashIndex].count == 0){
        hashTable[hashIndex].count = 1;
        hashTable[hashIndex].head = newNode;
    }
    
    else{
        hashTable[hashIndex].head->previous = newNode;
        newNode->next = hashTable[hashIndex].head;
        hashTable[hashIndex].head = newNode;
        hashTable[hashIndex].count++;
    }
}


char *search(char *key){
    int hashIndex = hashFunction(key);
    struct node* node = hashTable[hashIndex].head;
    int flag = 0;
    while (node != NULL)
    {
        if (node->key == key){
            flag = 1;
            break;
        }
        node = node->next;
    }
    if (flag == 1)
    	return node->value;

    else
        return "not found";

}

int main(int argc, char *argv[]){

	hashTable = (struct bucket *)calloc(BUCKET_SIZE, sizeof(struct bucket));

	struct hostent *myhost;
	struct in_addr myinaddr;
    
	struct hostent *host;
	struct sockaddr_in addr;
	int i = 0;
	
	
	int serv_sock;
	char buff_rcv[BUFSIZE+5];
	char buff_snd[BUFSIZE+5];
	
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	int clnt_addr_size;
	
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_DGRAM, 0);
	if(serv_sock==-1)
		error_handling("UDP socket create error");
	
	memset(&serv_addr, 0, sizeof(serv_addr)); 
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
		error_handling("bind() error");
	
	
	while(1){
		clnt_addr_size=sizeof(clnt_addr);
		recvfrom(serv_sock, buff_rcv, BUFSIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
		
		if((int)buff_rcv[0]>=48 && (int)buff_rcv[0] <= 57){
			if(strcmp(search(buff_rcv), "not found") != 0){
				sprintf(buff_snd, "%s", search(buff_rcv));
			}
			else{
				memset(&addr, 0, sizeof(addr));
				addr.sin_addr.s_addr=inet_addr(buff_rcv);

				host=gethostbyaddr((char*)&addr.sin_addr, 4, AF_INET);

				if(!host){	
					error_handling("ip not found");
				}
				
				add(host->h_name, buff_rcv);
				add(buff_rcv, host->h_name);
				
				sprintf(buff_snd, "%s", search(buff_rcv));
				
			}
		}else{
			if(strcmp(search(buff_rcv), "not found") != 0){
				sprintf(buff_snd, "%s", search(buff_rcv));
			}
			else{
				if((myhost = gethostbyname(buff_rcv)) == NULL){
					error_handling("domain not found");
					exit(1);
				}

				while(myhost->h_addr_list[i]!=NULL){
					myinaddr.s_addr=*((u_long*)(myhost->h_addr_list[i]));
					add(inet_ntoa(myinaddr), buff_rcv);
					i++;
				}
				add(buff_rcv, inet_ntoa(myinaddr));
				
				sprintf(buff_snd, "%s", search(buff_rcv));
			}
		}
	
		
		
		sendto(serv_sock, buff_snd, strlen(buff_snd)+1, 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
	}
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

