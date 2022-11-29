#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<ctype.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<errno.h>
#include<netdb.h>
#include<sys/wait.h>

#define PORT 21450
#define MAX 256
#define DEST_IP "127.0.0.1"

struct credentials
{
	char username[50];
	char password[50];
}users[300];

int main()
{

	char line[100]; 
	
	//To read a line from file
	//Reading the file and storing in a structure (Phase 0)
	
	char result[MAX]={'\0'},euser[MAX]={'\0'},epass[MAX]={'\0'};
	int i=0,len,n,n1,n2,sockfd,err=-13;
	int status = 0,u=0,p=0,comp=1;
	char Dummy_message[MAX];

	
	FILE *fp;       //File pointer
	char *sp;

	fp = fopen("cred.txt","r");
	if(fp == NULL)
	{
		printf("ERROR LOADING DATABASE!");
		exit(0);
	}

	else
	{
		i=0;
		//printf("\n DATA LOADED SUCCESSFULLY!\n");
		while ( fgets(line,100,fp) != NULL)
		{
			if((strlen(line)>0) && (line[strlen(line)-1] == '\n'))
					line[strlen(line)-1] = '\0';
			sp = strtok(line,",");
			strcpy (users[i].username,sp);
			sp = strtok(NULL,",");
			strcpy (users[i].password,sp);
			//printf("username : %d : %s Password: %s\n",i,users[i].username,users[i].password);
			i++;
		}
	}	

	struct sockaddr_in servaddr, cliaddr;

	//creating socket file descriptor
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0)
	{
		perror("Socket Creation failed!");
		exit(0);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	//Address Configuration
	servaddr.sin_family		 = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(DEST_IP);
	servaddr.sin_port		 = htons(PORT);

	//Binding the socket with server address
	if( bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr))<0)
	{
		perror("Bind failed!");
		exit(0);
	}

	printf("\nThe ServerC is up and running using UDP on port %d.\n",PORT);
	
	len = sizeof(cliaddr);
	int choice = 1;
	
	do
	{	
		// Clearing the Flags
		status == 0; u ==0; p == 0;
		//Clearing the strings
		memset(euser, 0, strlen(euser));
		memset(epass, 0, strlen(epass));

		memset(&servaddr, 0, sizeof(servaddr));
		memset(&cliaddr, 0, sizeof(cliaddr));

		//Receiving the username and Password
		n = recvfrom(sockfd, (char *)euser, MAX, MSG_WAITALL, (struct sockaddr*) &cliaddr, &len);
		euser[n] = '\0';
		//printf("ENCRYPTED USERNAME : %s.\n",euser); //username

		n1 = recvfrom(sockfd, (char *)epass, MAX, MSG_WAITALL, (struct sockaddr*) &cliaddr, &len);
		epass[n1] = '\0';
		//printf("ENCRYPTED PASSWORD : %s.\n",epass); //password

		printf("\n------------------------------------\n");
		printf("\nThe ServerC received an authentication request from the Main Server.");
		
		//Authentication Verification
		for(i=0;i<300;i++)
		{
			if(strcmp(euser,users[i].username)==0)
			{	
				u = 1;
				int comp = strcmp(epass,users[i].password);
				if(comp==0 || comp == err)
					{
						p = 1;
						status = 1;
						break;
					}
			}
		}

		if (status == 1 && u ==1 && p == 1)
			strcpy(result,"PASS");
		else if(u==0 && p==0)
			strcpy(result,"FAIL_NO_USER");
		else if(u==1 && p==0)
			strcpy(result,"FAIL_PASS_NO_MATCH");
		else
			printf("\nERROR!\n");

		//printf("\nResult: %s\n",result);
		//Sending the Results to serverM
		sendto(sockfd, (const char *) result, strlen(result), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len );
		printf("\nThe ServerC finished sending the response to the Main Server.\n");
	
	}while((status == 0) || (u == 0) || (p == 0));

	close(sockfd);
	return(0);
	
}