#include "TCPEchoServer.h"
#include <string.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<signal.h>
#include<fcntl.h>

void ProcessMain(int servSock, int clinetNumber);         /* Main program of process */

int clients[100];
char *http_root_dir_name;

int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server*/
    unsigned short echoServPort;     /* Server port */
    pid_t processID;                 /* Process ID */
    unsigned int processLimit = 5;       /* Number of child processes to create */
    unsigned int processCt;          /* Process counter */
    int clientNumber = 0;

    
    if (argc != 3)     /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage:  %s <SERVER PORT> <directory>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg: local port */
//    processLimit = atoi(argv[2]);  /* Second arg:  number of child processes */

    http_root_dir_name = (char*)malloc(strlen(argv[2]));
    strcpy(http_root_dir_name,argv[2]);
    servSock = CreateTCPServerSocket(echoServPort);

    int i;
    for (i=0; i<100; i++)
        clients[i]=-1;

    for (processCt=0; processCt < processLimit; processCt++){
        /* Fork child process and report any errors */
        if ((processID = fork()) < 0)
            DieWithError("fork() failed");
        else if (processID == 0)  /* If this is the child process */
            ProcessMain(servSock, clientNumber);
	while (clients[clientNumber]!=-1) 
		clientNumber = (clientNumber+1);
    }

    exit(0);  /* The children will carry on */
}

void ProcessMain(int servSock, int clientNumber)
{
    int clntSock;                  /* Socket descriptor for client connection */

    for (;;)  /* Run forever */
    {	
        clntSock = AcceptTCPConnection(servSock);
	clients[clientNumber] = clntSock;
        printf("with child process: %d\n", (unsigned int) getpid());
        HandleTCPClient(clientNumber, clients, http_root_dir_name);
    }    
}
