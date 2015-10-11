#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
typedef struct file {

	char name[1024];
	char time[1024];
	int size;
	char type[100];
}fs;

int Client(int clientPortNo, int flag)
{

	int ClientSocket = 0;
	struct sockaddr_in server_addr;
    struct hostent *host;

	// Creating a socket
	if(flag==0)
		ClientSocket = socket(AF_INET,SOCK_STREAM,0);
	else		
		ClientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(ClientSocket<0)
	{
		printf("ERROR WHILE CREATING A SOCKET\n");
		return 1;
	}
	else
		printf("[CLIENT] Socket created \n");
memset(&server_addr, 0, sizeof(server_addr));                /* zero the struct */

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(clientPortNo);
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);

	if(flag==0)
	while(connect(ClientSocket,(struct sockaddr *)&server_addr,sizeof(server_addr))<0);

	while(1)
	{
		char cmd[1024];

		char c;
		char command[20][100];
		int count = 0;

		scanf("%c", &c);

		while(c!='\n')
		{
			cmd[count++] = c;
			scanf("%c",&c);
		}

		cmd[count++] = '\0';
		int commandCount = 0,i;

		count = 0;

		for (i=0 ; i<strlen(cmd); i++)
		{
			if (cmd[i] == ' ')
			{
				command[commandCount][count++] = '\0';
				commandCount++;
				count = 0;
				continue;
			}
			command[commandCount][count] = cmd[i];
			count++;
		}
		command[commandCount][count++] = '\0';
		commandCount++;
		char send_data[1024],recv_data[1024];
					int bytes_recieved;

		if(strcmp(command[0],"exit")==0)
			break;
		else if(strcmp(command[0], "download")==0)
		{
			if(commandCount<2)
			{

			}
			else 
			{
				strcpy(send_data,"D ");
				strcat(send_data,command[1]);
				int p;
				if(flag==0)
					send (ClientSocket, send_data,1024, 0);
				else
					p=sendto(ClientSocket, send_data, 1024, 0,(struct sockaddr *)&server_addr, sizeof(server_addr));

				printf("%d\n",p );
				FILE *fp1 = fopen(command[1],"w");
				while(1){
					bzero(recv_data, 1025);
					if(flag==0)
						bytes_recieved = recv(ClientSocket, recv_data, 1024, 0);
					else
						bytes_recieved=recvfrom(ClientSocket,recv_data,1024,0,(struct sockaddr *)&server_addr, sizeof(server_addr));
					printf("Bytes: %d\n",bytes_recieved);
					//recv_data[bytes_recieved] = '\0';
					printf("%s\n",recv_data );
					fwrite(recv_data, sizeof(recv_data[0]),strlen(recv_data), fp1);
					if(bytes_recieved < 1024)
					{
						break;
					}	
				}
				printf("=========== File Download Complete ===========\n");
				fclose(fp1);


			}

		}
		else if(strcmp(command[0], "FileUpload")==0)
		{
			// char send_data[1024],recv_data[1024];
			// int bytes_recieved;
			printf("%s\n",command[0] );
			if (commandCount < 2)
			{
				printf("yuppppppp\n");
			}
			else
			{

				strcpy(send_data,"U ");
				strcat(send_data,command[1]);
				if(flag==0)
				{
					send (ClientSocket, send_data,1024, 0);
					printf("%s\n",send_data );

					bytes_recieved=recv(ClientSocket,recv_data,1024,0);
				}
				else
				{
					sendto(ClientSocket, send_data, 1024, 0,(struct sockaddr *)&server_addr, sizeof(server_addr));
					perror("sendto");
					bytes_recieved=recvfrom(ClientSocket,recv_data,1024,0,(struct sockaddr *)&server_addr, sizeof(server_addr));
					printf("%s\n",recv_data);
				}
				recv_data[bytes_recieved] = '\0';
				if(recv_data[0]=='y')
				{
					char arr[100];
					strcpy(arr,command[1]);
					//strcat(arr,pch);
					printf("%s\n",arr);
					FILE *fp = fopen(arr,"r");
					if(fp == NULL)
					{
						printf("wrong file\n");
						continue;
					}

					//=================told the client that file is to be sent========================
					//fgets ( data, sizeof(data), fp ) != NULL
					// char data[1024];
					memset(send_data,0,1024);
					int byteR,sentN;

					while(!feof(fp))
					{	//memset(send_data,0,1024);
						byteR = fread(send_data,1,1024,fp);
						if(flag==0)
							sentN =  write(ClientSocket,send_data,byteR);
						else
							sentN=sendto(ClientSocket, send_data, byteR, 0,(struct sockaddr *)&server_addr, sizeof(server_addr));

						//===============================file sent=================================================
					}
					//		printf("End file\n");
					memset(send_data,0,1024);

					/*char end[]= "End Of File";
					strcpy(send_data,end);
					if(flag==0)
						write(ClientSocket,send_data,1024);
					else
						sendto(ClientSocket, send_data, 1024, 0,(struct sockaddr *)&client_addr, sizeof(struct sockaddr));*/
					fclose(fp);
				}
				else
					printf("fileupload denied :(");

			}	
		}

	}
	printf("Closing Connection\n");
	close(ClientSocket);
	return 0;
}
int Server(int serverPortNo, int flag)
{
	int listenSocket = 0;	// This is my server's socket which is created to 
	//	listen to incoming connections
	int listen1=0;
	int connectionSocket = 0;

	struct sockaddr_in server_addr,client_addr;		// This is for addrport for listening

	// Creating a socket
	if(flag==0)
		listenSocket = socket(AF_INET,SOCK_STREAM,0);
	else
		listenSocket = socket(AF_INET, SOCK_DGRAM, 0); 
	if(listenSocket<0)
	{
		printf("ERROR WHILE CREATING A SOCKET\n");
		return 1;
	}
	else
		printf("[SERVER] SOCKET ESTABLISHED SUCCESSFULLY\n\n");

	// Its a general practice to make the entries 0 to clear them of malicious entry

	// bzero((char *) &server_addr,sizeof(server_addr));

	// Binding the socket
memset(&server_addr, 0, sizeof(server_addr));     
// memset(&client_addr, 0, sizeof(client_addr));                /* zero the struct */
           /* zero the struct */
// memset(&server_addr, 0, sizeof(dest));                /* zero the struct */

	server_addr.sin_family = AF_INET;
	int yoo=server_addr.sin_family;	//For a remote machine
	server_addr.sin_addr.s_addr =htons(INADDR_ANY);
	server_addr.sin_port = htons(serverPortNo);
	int addr_len = sizeof(struct sockaddr);

if(flag==0)
	bzero(&(server_addr.sin_zero),8); 

	if(bind(listenSocket,(struct sockaddr * )&server_addr,sizeof(server_addr))<0)
	{
		printf("ERROR WHILE BINDING THE SOCKET\n");
		return 1;
	}
	else
		printf("[SERVER] SOCKET BINDED SUCCESSFULLY\n");

	// Listening to connections
if(flag==0)
	if(listen(listenSocket,10) == -1 )	//maximum connections listening to 10
	{
		printf("[SERVER] FAILED TO ESTABLISH LISTENING \n\n");
		return 1;
	}
	printf("[SERVER] Waiting fo client to connect....\n" );

	// Accepting connections
	if(flag==0)
	while((listen1=accept(listenSocket , (struct sockaddr*)NULL,NULL))<0 );

	// NULL will get filled in by the client's sockaddr once a connection is establised

	printf("[CONNECTED]\n");
	char cmd[1024],recv_data[1024],send_data[1024];
	int bytes_recieved;
	 socklen_t len = sizeof(client_addr);

	while(1)
	{
 int len = sizeof client_addr;

				printf("%d\n",client_addr.sin_family );

		if(flag==0)
			bytes_recieved=recv(listen1,recv_data,1024,0);
		else
			bytes_recieved=recvfrom(listenSocket,recv_data,1024,0,(struct sockaddr *)&client_addr, &len);
		    perror("recvfrom");

		recv_data[bytes_recieved] = '\0';
		printf("%s\n",recv_data);
		if(recv_data[0]=='D')
		{
			printf("yoooooooo\n");
			char arr[100];
			strcpy(arr,recv_data+1);
			//strcat(arr,pch);
			printf("Sending %s\n",arr);
			FILE *fp = fopen(recv_data+2,"r");
			if(fp == NULL)
			{
				printf("wrong file\n");
				continue;
			}

			//=================told the client that file is to be sent========================
			//fgets ( data, sizeof(data), fp ) != NULL
			// char data[1024];
			memset(send_data,0,1024);
			int byteR,sentN;
	// server_addr.sin_family = yoo;

			while(!feof(fp))
			{	//memset(send_data,0,1024);
				byteR = fread(send_data,1,1024,fp);
				send_data[byteR]='\0';
				printf("%s\n",send_data );
				if(flag==0)
				  write(listen1,send_data,byteR);
				else
					sentN=sendto(listenSocket, send_data, 1023, 0,(struct sockaddr *)&client_addr, sizeof(client_addr));
						printf("%d\n",client_addr.sin_family );

				fflush(stdout);
				    printf("Error sending msg: %s\n", strerror(errno));

				// printf("%d\n",listenSocket );

				printf("%d\n",sentN );

				//===============================file sent=================================================
			}
			//		printf("End file\n");
			memset(send_data,0,1024);

			/*char end[]= "End Of File";
			strcpy(send_data,end);
			if(flag==0)
				write(listenSocket,send_data,1024);
			else
				sendto(listenSocket, send_data, 1024, 0,(struct sockaddr *)&client_addr, sizeof(struct sockaddr));*/
			fclose(fp);
		}
		else if(recv_data[0]=='U')
		{
			printf("[Y/y] for Accept\n[N/n] for Cancel\n" );
			char ar[10];
			scanf("%s",ar);
			strcpy(send_data,ar);
			printf("%s\n",send_data );
			if(flag==0)
				send (listen1, send_data,1024, 0);
			else
				sendto(listenSocket, send_data, 1024, 0,(struct sockaddr *)&client_addr, sizeof(client_addr));

			if(ar[0]=='y')
			{
				FILE *fp1 = fopen(recv_data+2,"w");
				memset(recv_data,0,1024);
				while(1){
					if(flag==0)
						bytes_recieved=recv(listen1, recv_data, 1024, 0);
					else
						bytes_recieved=recvfrom(listenSocket,recv_data,1024,0,(struct sockaddr *)&client_addr, &len);
					printf("Bytes: %d\n",bytes_recieved);
					printf("%s\n",recv_data );
					fwrite(recv_data, 1,strlen(recv_data), fp1);
					if(bytes_recieved < 1024)
					{
						break;
					}	
				}
				printf("========== File Recieved ==========\n");
				fclose(fp1);
			}


		}

	}
	return 0;
}
int main()
{
	int servPortNo, clientPortNo;
	char type[20];
	printf("Port to listen to : ");
	scanf("%d",&servPortNo);

	printf("Port to send data : ");
	scanf("%d",&clientPortNo);
	printf("for tcp type 1 and for udp type 2\n");
	int flag;
	scanf("%d",&flag);
	pid_t pid;

	pid = fork();

	if (pid == -1)
	{
		printf ("\n #Error forking process\n");
		exit(1);
	}
	if (pid == 0)									//Server on child thread.
	{
		Server(servPortNo, flag - 1);

	}
	else											//Client on parent thread.
	{
		while(1)
		{
			int p;
			p = Client(clientPortNo, flag - 1);
			if (p <= 0)
			{
				break;
			}
			sleep(0);								//Keep pinging at 1sec intervals.
		}

	}
	kill(pid, SIGQUIT);
	return 0;

}
