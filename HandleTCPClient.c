#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/stat.h>
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */
#include <string.h>
#include<fcntl.h>

#define buffer 1024

void HandleTCPClient(int n, int clients[], char *http_root_dir_name)
{
    char mesg[99999], *requestLine[3], data[buffer], path[99999];
    int rcvd, fd, buffer_read;
    long fileSize;
    struct stat stat_result;

    memset( (void*)mesg, (int)'\0', 99999 );

    rcvd=recv(clients[n], mesg, 99999, 0);

    if (rcvd<0)    // receive dieWithError
        fprintf(stderr,("recv() dieWithError\n"));
    else if (rcvd==0)    // receive socket closed
        fprintf(stderr,"Client disconnected upexpectedly.\n");
    else    // message received
    {
        printf("%s", mesg);
        requestLine[0] = strtok (mesg, " \t\n");
        if ( strncmp(requestLine[0], "GET\0", 4)==0 )
        {
            requestLine[1] = strtok (NULL, " \t");
            requestLine[2] = strtok (NULL, " \t\n");
            if ( strncmp( requestLine[2], "HTTP/1.0", 8)!=0 && strncmp( requestLine[2], "HTTP/1.1", 8)!=0 )
            {
                write(clients[n], "HTTP/1.0 400 Bad Request\n", 25);
            }
            else
            {
                if ( strncmp(requestLine[1], "/\0", 2)==0 )
                    requestLine[1] = "/index.html";        //Because if no file is specified, index.html will be opened by default (like it happens in APACHE...

                strcpy(path, http_root_dir_name);
                strcpy(&path[strlen(http_root_dir_name)], requestLine[1]);
                printf("file: %s\n", path);

                if ( (fd=open(path, O_RDONLY))!=-1 )    //FILE FOUND
                {
		    stat(path, &stat_result);
		    printf("\nfile size = %ld\n", stat_result.st_size);
                    send(clients[n], "HTTP/1.0 200 OK\n\n", 17, 0);
                    while ( (buffer_read=read(fd, data, buffer))>0 )
                        write (clients[n], data, buffer_read);
                }
                else    send(clients[n], "HTTP/1.0 404 Not Found\n", 23, 0); //FILE NOT FOUND
            }
        }
    }

    //Closing SOCKET
    shutdown (clients[n], SHUT_RDWR);         //All further send and recieve operations are DISABLED...
    close(clients[n]);
    clients[n]=-1;
}
