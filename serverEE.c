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

#define Port 23450
#define MAX 256
#define DEST_IP "127.0.0.1"

struct EE
{
	char CourseCode[MAX];
	char Prof[MAX];
	char Days[MAX];
	char CourseName[MAX];
	int  credits;
}ee[300];


int main()
{
	int sockfd,len,n,n1,cat=0,found =0,i=0;
	char line[100]; 

	//To read a line from file
	//Reading the file and storing in a structre
	
	FILE *fp;
	char *sp;
	char coursecode[MAX]={'\0'},category[MAX]={'\0'};

	//opening File
	fp = fopen("ee.txt","r");
	if(fp == NULL)
	{
		printf("ERROR LOADING DATABASE!");
		exit(0);
	}

	else
	{
		i=0;
		while ( fgets(line,100,fp) != NULL)
		{
			sp = strtok(line,",");
			strcpy (ee[i].CourseCode,sp); 
			
			sp = strtok(NULL,",");
			ee[i].credits = atoi(sp);

			sp = strtok(NULL,",");
			strcpy (ee[i].Prof,sp);

			sp = strtok(NULL,",");
			strcpy (ee[i].Days,sp);

			sp = strtok(NULL,",");
			strcpy (ee[i].CourseName,sp);
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

	//Address configuration
	servaddr.sin_family		 = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(DEST_IP);
	servaddr.sin_port		 = htons(Port);

	//Binding the socket with server address
	if( bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr))<0)
	{
		perror("Bind failed!");
		exit(0);
	}

	printf("\nThe Server EE is up and running using UDP on port %d.\n",Port);
	
	int choice = 1;
	len = sizeof(cliaddr);
	
	while(choice == 1)
	{
		choice = 0; 
		cat = 0,found =0;
	
		//Receiving the coursecode and category to process the query
		n = recvfrom(sockfd, (char *)coursecode, MAX, MSG_WAITALL, (struct sockaddr*) &cliaddr, &len);
		coursecode[n] = '\0';

		n1 = recvfrom(sockfd, (char *)category, MAX, MSG_WAITALL, (struct sockaddr*) &cliaddr, &len);
		category[n1] = '\0';

		//New Request starting
		printf("\n-----Start a new request-----\n");
		printf("\nThe Server EE received a request from the Main Server about the %s of %s.",category,coursecode);

		//Categorizing the requests
		if(strcasecmp (category,"Credit")==0)
			cat = 1;
		else if(strcasecmp(category,"Professor")==0)
			cat = 2;
		else if(strcasecmp (category,"Days")==0)
			cat = 3;
		else if(strcasecmp (category,"Coursename")==0)
			cat = 4;	
		else
			cat = 0;	

		for(i=0;i<300;i++)
		{
			if(strcmp(coursecode,ee[i].CourseCode) == 0)
			{
				if (cat == 1 )
					{
						printf("\nThe course information has been found: The %s of %s is %d",category,coursecode,ee[i].credits);
						int cred = ee[i].credits;
						sendto(sockfd, &cred, sizeof(cred), 0, (struct sockaddr *) &cliaddr, len);
						found = 1;
						break;
					}
				else if (cat == 2)
					{
						printf("\nThe course information has been found: The %s of %s is %s",category,coursecode,ee[i].Prof);
						sendto(sockfd, (const char *) ee[i].Prof, strlen(ee[i].Prof), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len );
						found = 1;
						break;
					}
				else if (cat == 3)
					{
						printf("\nThe course information has been found: The %s of %s is %s",category,coursecode,ee[i].Days);
						sendto(sockfd, (const char *) ee[i].Days, strlen(ee[i].Days), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len );
						found = 1;
						break;
					}
				else if (cat == 4)
					{
						printf("\nThe course information has been found: The %s of %s is %s",category,coursecode,ee[i].CourseName);
						sendto(sockfd, (const char *) ee[i].CourseName, strlen(ee[i].CourseName), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len );
						found = 1;
						break;
					}
				else
					printf("\nDidn't find the course : %s\n",coursecode);
			}
		}
		if(found == 0)
		{
			printf("Didn't find the course : %s\n",coursecode);
			char error_message[MAX] = "Not Found!";
			sendto (sockfd, (const char *) error_message, strlen(error_message), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len );
		}
		//Receiving the choice to help close the connections
		printf("\nThe Server EE finished sending the response to the Main Server.\n");
		recvfrom(sockfd, &choice, sizeof(choice), 0, (struct sockaddr *) &cliaddr, &len);
	}

	close(sockfd);
	fclose(fp);
	return (0);		
}