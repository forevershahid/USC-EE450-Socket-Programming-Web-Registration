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

#define DEST_IP "127.0.0.1"
#define Port 25450
#define MAX 256

int main()
	{
		int client_socket,n=3,encode=0;
		int choice =1,cred,i=0;
		char Dummy_response[MAX],coursecode[MAX]={'\0'},category[MAX]={'\0'}; 
		char username[MAX]={'\0'},password[MAX]={'\0'},result[MAX]={'\0'},information[MAX]={'\0'};

		//Socket creation
		client_socket = socket(AF_INET, SOCK_STREAM, 0); //TCP

		//Address specification
		struct sockaddr_in server_address, local_address;

		server_address.sin_family	= AF_INET;
		server_address.sin_port 	= htons(Port); // Change it back to Port
		server_address.sin_addr.s_addr	= inet_addr(DEST_IP);

		//Connection of socket with server address
		int status = connect (client_socket, (struct sockaddr*) &server_address, sizeof(server_address));
		if(status == -1)
		{
			printf("Connection Error! Try again.");
			exit(0);
		}
		else
			printf("\nThe Client is UP and Running!");

		//Dynamic Port Allocation
		int addr_size = sizeof(local_address);
		getsockname(client_socket, (struct sockaddr*)&local_address, &addr_size);
		//printf("[%s:%u] > ",inet_ntoa(local_address.sin_addr),ntohs(local_address.sin_port));

		do
		{	
			//Clearing the Buffer
			memset (username, 0 ,strlen(username));
			memset (password, 0, strlen(password));

			//Accepting Inputs till the First space
	        printf("\nPlease Enter the Username: ");
	        scanf("%s",username);
	        printf("Please Enter the Password: ");
	        scanf("%s",password);
			
			//Calculating Constraints
			int userlen = strlen(username);
			int passlen = strlen(password);
			if(userlen <5 | userlen>50 | passlen<5 | passlen>50)
				{
					printf("\nINVALID LENGTH OF USERNAME/PASSWORD\n");
					printf("\nClient shuts down due to constraint Error!\n");
					exit(0);
				}
			int ascii=0;
			for(i =0;i<userlen;i++)
				{
					if(isalpha(username[i]))
						{
						ascii = username[i];
						if((ascii >=97 && ascii <=122)!=1)
							{
								printf("\n INVALID USERNAME !\n");
								printf("\nClient shuts down due to constraint Error!\n");
								exit(0);
							}
						}
					else
						{
							printf("\n INVALID USERNAME !\n");
							printf("\nClient shuts down due to constraint Error!\n");
							exit(0);
						} 
				}

			//printf("\nUsername entered: %s\n",username);
			//printf("Password entered: %s\n",password);
			//printf("\n");

			//SENDING USERNAME AND PASSWORD TO MAIN SERVER
			send(client_socket, username, sizeof(username), 0);
			send(client_socket, password, sizeof(password), 0);
			
			printf("\n%s sent an Authentication Request to the Main Server",username);
			
			// RECEIVING AUTHENTICATION RESULT FROM MAIN SERVER
			recv(client_socket, result, sizeof(result),0); 

			// RESULT MAPPING
			if (strcmp("PASS",result)==0)
				encode = 0;
			else if (strcmp("FAIL_NO_USER",result)==0)
				encode = 1;
			else
				encode = 2;
			
			//ATTEMPTS CALCULATION
			if(encode == 0)
				{
					printf("\n%s Received the result of authentication using TCP over port %u.Authentication is successful!\n",username,ntohs(local_address.sin_port));
					n = 0;
					break;
				}
			else if(encode == 1)
			{
				n = n-1;
				printf("\n%s Received the result of authentication using TCP over port %u.\nAuthentication failed: Username Does not exist.\nNo of Attempts remaining is %d.\n", username,ntohs(local_address.sin_port),n);
			}
			else if(encode == 2)
			{
				n = n-1;
				printf("\n%s Received the result of authentication using TCP over port %u.\nAuthentication failed: Password does not match.\nNo of Attempts remaining is %d.\n", username,ntohs(local_address.sin_port),n);
			}

			else if (n==0)
			{
				printf("\nAuthentication failed for 3 Attempts, Client is shutting down!\n");
				exit(0);
			}
			else
				exit(0);
		}while ((strcmp("PASS",result)!=0) && (n!=0));

		//Attempts finished
		if(encode != 0 && n == 0)
		{
			printf("\nAuthentication failed for 3 Attempts, Client is shutting down!\n");
			exit(0);
		}

		//If Authenticated Successfully
		do
		{
			if(n==0 & choice == 1)
			{
				memset (coursecode, 0, sizeof(coursecode));
				memset (category, 0, sizeof(category));
				
				printf("\n-----Start a new request-----\n");

				printf("\nPlease Enter the Course Code to Query: ");
				scanf("%s",coursecode);
				printf("Please enter the category (Credit / Professor / Days / Coursename): ");
				scanf("%s",category);

				send(client_socket, coursecode, sizeof(coursecode), 0);

				recv(client_socket, Dummy_response, sizeof(Dummy_response), 0); //Dummy

				send(client_socket, category, sizeof(category), 0);
				printf("\n%s sent a request to the Main Server",username);


				printf("\nThe client received the response from the Main server using TCP over port %u.",ntohs(local_address.sin_port));

				if(strcmp("Credit",category)==0)
				{

					int credit = recv (client_socket, &cred , sizeof(cred), 0);
					printf("\nThe %s of %s is %d.\n",category,coursecode,credit);
				}
				else
				{
					recv(client_socket, information, sizeof(information), 0);
					printf("\nThe %s of %s is %s.\n",category,coursecode,information);
				}
				printf("\nProceed with new request? (0 - No, 1 - Yes): ");
				scanf("%d",&choice);

				
				send(client_socket, &choice, sizeof(choice), 0);
			}
			else
				exit(0);

		// Intake of choice helps close the other terminals.
		}while(choice!=0);

		close(client_socket);
		return 0;
}
