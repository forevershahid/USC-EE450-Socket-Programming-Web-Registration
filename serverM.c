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

#define TCPP 25450
#define PORT 24450
#define PORTC 21450
#define PORTCS 22450
#define PORTEE 23450
#define DEST_IP "127.0.0.1"

#define MAX 256

int main()
{
	int sockfd,server_socket,network_socket,sockfdcs,sockfdee,choice =1,comp=1;
	char result[MAX]={'\0'},username[MAX]={'\0'},password[MAX]={'\0'},coursecode_recv[MAX]={'\0'};
	char euser[MAX]={'\0'},epass[MAX]={'\0'},coursecode[MAX]={'\0'},category[MAX]={'\0'},information[MAX]={'\0'};
	int i=0,encode=0,n1,cred,n,len,udpclient;
	char Dummy_message[MAX]= "Dummy";
 	
 	struct sockaddr_in servaddr,server_address, servcsaddr,serveeaddr,client_addr,udp_client,servaddrudp;

	memset(&servaddr , 0, sizeof(servaddr));

	memset(&servcsaddr , 0, sizeof(servcsaddr));

	memset(&serveeaddr , 0, sizeof(serveeaddr));

	memset(&servaddrudp, 0, sizeof(servaddrudp));

	//Socket file descriptor for client TCP
	if ((server_socket = socket(AF_INET, SOCK_STREAM,0)) < 0)
	{
		perror("Socket Creation Failed!");
		exit(0);
	}

	if((udpclient = socket(AF_INET, SOCK_DGRAM,0))<0)
	{
		perror("Socket Creation Failed!");
		exit(0);
	}

	//serverM UDP PORT Configuration
	udp_client.sin_family 				= AF_INET;
	udp_client.sin_port					= htons(PORT);
	udp_client.sin_addr.s_addr 			= inet_addr(DEST_IP);

	//client TCP PORT Configuration
	server_address.sin_family 			= AF_INET;
	server_address.sin_port				= htons(TCPP);
	server_address.sin_addr.s_addr 		= inet_addr(DEST_IP);

	//serverC UDP PORT Configuration
	servaddr.sin_family				 	= AF_INET;
	servaddr.sin_port				 	= htons(PORTC);
	servaddr.sin_addr.s_addr 		 	= inet_addr(DEST_IP);

	//serverCS UDP PORT Configuration
	servcsaddr.sin_family			 	= AF_INET;
	servcsaddr.sin_port		 		 	= htons(PORTCS);
	servcsaddr.sin_addr.s_addr 		 	= inet_addr(DEST_IP);

	//serverEE UDP PORT Configuration
	serveeaddr.sin_family		 	 	= AF_INET;
	serveeaddr.sin_port				 	= htons(PORTEE);
	serveeaddr.sin_addr.s_addr 		 	= inet_addr(DEST_IP);

	//Binding the UDP port to the address of serverM
	if(bind (udpclient , (struct sockaddr*)&udp_client, sizeof(servaddrudp))<0)
	{
		printf("\nSocket Bind Failed!\n");
	}

	//Binding the TCP port
	if(bind (server_socket, (struct sockaddr*) &server_address, sizeof (server_address))!=0)
	{
		printf("\nSocket Bind Failed!\n");
	}

	printf("\nThe main server is up and running\n");
	
	//Listening for incoming connections on the TCP Parent socket
	if(listen (server_socket,5)!=0)
	{
		printf("\nListen Failed!\n");
	}

	//Child TCP socket accepting incming connection
	int network_size	= sizeof(client_addr);
	network_socket	= accept(server_socket, (struct sockaddr*)&client_addr, &network_size);

	do
	{
		//Clearing the Buffer to avoid garbage
		memset(euser, 0, strlen(euser));
		memset(epass, 0, strlen(epass));

		memset(username, 0, strlen(username));
		memset(password, 0, strlen(password));

		//Receiving Username and Password from the TCP client
		recv (network_socket, username, sizeof(username), 0);
		recv (network_socket, password, sizeof(password), 0);

		printf("\nThe main server received the authentication for %s using TCP over port %d.",username, TCPP);

		int userlen = strlen(username);
		int passlen = strlen(password);

		//Username Encryption
		for(i=0;i<userlen;i++)
		{
			if(isalnum(username[i]))
				{
					int ascii = 0;
					ascii = username[i];
					ascii = ascii + 4;
					if ((ascii>=91 && ascii<=94) | (ascii>=123 && ascii<=126))
						ascii = ascii - 26;
					if ((ascii>=58 && ascii<=61))
						ascii = username[i] - 6;	
					euser[i] = ascii;
				}
			else
					euser[i] = username[i];
		}

		// Password Encryption
		for(i=0;i<passlen;i++)
		{
			 if(isalnum(password[i]))
			 	{
					int ascii = 0;
					ascii = password[i];
					ascii = ascii +4;
					if ((ascii>=91 && ascii<=94) | (ascii>=123 && ascii<=126))
						ascii = ascii -26;
					if ((ascii>=58 && ascii<=61))
						ascii = password[i] - 6;
					epass[i] = ascii;
				}
			else
					epass[i] = password[i];
		}

		//SENDING AUTHENTICATION INFORMATION TO SERVER C TO AUTHENTICATE
		sendto(udpclient, (const char *)euser , strlen(euser), MSG_CONFIRM, (const struct sockaddr*) &servaddr, sizeof(servaddr));

		sendto(udpclient, (const char *)epass , strlen(epass), MSG_CONFIRM, (const struct sockaddr*) &servaddr, sizeof(servaddr));
		
		printf("\nThe main server sent an authentication request to serverC.");

		//RECEIVING RESULT OF AUTHENTICATION
		n = recvfrom(udpclient, (char *)result, MAX, MSG_WAITALL, (struct sockaddr*) &servaddr, &len);
		result[n] = '\0';

		//printf("\n result: %s\n",result);
		printf("\nThe main server received the result of the authentication request from ServerC using UDP over port %d.",PORT);

		//SENDING RESULT OF AUTHENTICATION TO CLIENT
		send (network_socket, result, sizeof(result),0);
		printf("\nThe main server sent the authentication result to the client.");
		comp = strcmp("PASS",result);
	}while(comp !=0);

	do
	{

		printf("\n----------------------------------\n");
		encode = 0;

		//RECEIVING QUERY
		recv (network_socket, coursecode, sizeof(coursecode), 0);

		//Receiving dummy message
		send (network_socket, Dummy_message, sizeof(Dummy_message),0); //dummy
		
		recv (network_socket, category, sizeof(category), 0);

		printf("\nThe main server received from %s to query course %s about %s using TCP over port %d.",username,coursecode,category,TCPP);

		//Checking the First two chars of coursecode to determine the Subject/Department
		if(coursecode[0]=='E' && coursecode[1]=='E')
				encode = 1;
		else if(coursecode[0]=='C' && coursecode[1]=='S')
				encode = 2;
		else
			{
				printf("\nDepartment not Found!\n");
				encode = 0;
			}
		

		//PROCESSING QUERY
		if (encode == 1)
		{
			//Sending the query the EE dept Server
			sendto(udpclient, (const char *)coursecode , strlen(coursecode), MSG_CONFIRM, (const struct sockaddr*) &serveeaddr, sizeof(serveeaddr));
			sendto(udpclient, (const char *)category , strlen(category), MSG_CONFIRM, (const struct sockaddr*) &serveeaddr, sizeof(serveeaddr));
			printf("\nThe Main server sent a request to ServerEE");
			
			if(strcmp("Credit",category)==0)
			{
				//Receiving the Credit Query
				int ncred = recvfrom(udpclient, &cred, sizeof(cred), 0, (struct sockaddr *) &serveeaddr, &len);
				//printf("\n INT VALUE OF CREDITS: %d\n",cred);
				printf("\nThe main server received the response from server EE using UDP over port %d.",PORT);
				send(network_socket, &cred, sizeof(cred),0);
				printf("\nThe main server sent the query information to the client.");
			}
			else
			{	
				//Receiving general information
				n1 = recvfrom(udpclient, (char *)information, MAX, MSG_WAITALL, (struct sockaddr*) &serveeaddr, &len);
				information[n1] = '\0';
				//printf("\n Information: %s\n",information);
				printf("\nThe main server received the response from server EE using UDP over port %d.",PORT);
				send(network_socket, information, sizeof(information),0);
				printf("\nThe main server sent the query information to the client.");
			}

		}
		else if(encode == 2)
		{
			//Sending the query the CS dept Server
			sendto(udpclient, (const char *)coursecode , strlen(coursecode), MSG_CONFIRM, (const struct sockaddr*) &servcsaddr, sizeof(servcsaddr));
			sendto(udpclient, (const char *)category , strlen(category), MSG_CONFIRM, (const struct sockaddr*) &servcsaddr, sizeof(servcsaddr));
			printf("\nThe Main server sent a request to ServerCS");

			if(strcmp("Credit",category)==0)
			{
				//Receiving the Credit Query
				int ncred = recvfrom(udpclient, &cred, sizeof(cred), 0, (struct sockaddr *) &servcsaddr, &len);
				//printf("\n INT VALUE OF CREDITS: %d\n",cred);
				printf("\nThe main server received the response from server CS using UDP over port %d.",PORT);
				send(network_socket, &cred, sizeof(cred),0);
				printf("\nThe main server sent the query information to the client.");
			}
			else
			{	
				//Receiving general information
				n1 = recvfrom(udpclient, (char *)information, MAX, MSG_WAITALL, (struct sockaddr*) &servcsaddr, &len);
				information[n1] = '\0';
				//printf("\n Information: %s\n",information);
				printf("\nThe main server received the response from server CS using UDP over port %d.",PORT);
				send(network_socket, information, sizeof(information),0);
				printf("\nThe main server sent the query information to the client.");
			}
			
		}
		else
		{
			printf("\nINVALID ENTRY!\n");
		}

		
		recv (network_socket, &choice, sizeof(choice), 0);

		//Closing Command
		if(encode == 1)
			sendto(udpclient, &choice , sizeof(choice), 0, (struct sockaddr*) &serveeaddr, sizeof(serveeaddr));
		if(encode == 2)
			sendto(udpclient, &choice , sizeof(choice), 0, (struct sockaddr*) &servcsaddr, sizeof(servcsaddr));

	}while(choice != 0);
	
	//CLosing all active sockets
	close(sockfd);
	close(server_socket);
	close(network_socket);
	return(0);
	
}