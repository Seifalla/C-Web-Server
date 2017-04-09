#compiling the program
server3712:
	g++ *.c -o server3712

#defining the object files
OBJECTS = CreateTCPServerSocket.c  HandleTCPClient.c  TCPEchoServer-ForkN.c AcceptTCPConnection.c    DieWithError.c 
#main.o
HEADERS = TCPEchoServer.h

server3712: $(OBJECTS)
	g++ $^ -o server3712
%.o: %.c $(HEADERS)
	g++ -c $< -o $@

#cleaning up the trash
clean:
	rm -f *.o server3712
