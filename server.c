#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "1030"
#define BUFFER_SIZE 1024
#define QUEUE_SIZE 1


int main() {
  int sockFD, newSockFD, portNo, val;
  struct sockaddr_in servAddr, clientAddr;
  char buffer[BUFFER_SIZE];
  socklen_t len;        // stores sizeof address

  portNo = atoi(PORT);  // convert string to its equivalent int.
  sockFD = socket(AF_INET, SOCK_STREAM, 0);  
                        // create socket (IPv4, TCP/Stream Socket, IP protocol)
                        // sockFD = socket file descriptor

  if (sockFD < 0) {     // check if socket is created or not
    printf("Error in creating socket !\n");
    exit(1);
  }else {
    printf("Server Started...\n\n");
  }

  // building the structure for server address
  servAddr.sin_family = AF_INET;          // store address family
  servAddr.sin_addr.s_addr = INADDR_ANY;  // use my IPv4 address (local IP address)
  servAddr.sin_port = htons(portNo);      // host to network short

  // handle "Address Already in use" error.
  if (setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
    perror("setsockopt");
    exit(1);
  }

  // bind the socket
  if (bind(sockFD, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
    printf("Error in binding socket !\n");
    exit(1);
  }

  // listem to any incoming client request
  listen(sockFD, QUEUE_SIZE);

  len = sizeof(clientAddr);
  // When a connection arrives, open a new socket to communicate with it
  newSockFD = accept(sockFD, (struct sockaddr *) &clientAddr, &len);

  if (newSockFD < 0) {
    printf("Error in accepting request !\n");
    exit(1);
  }

  while (1) {
    bzero(buffer, BUFFER_SIZE);                 // clear the buffer content
    val = read(newSockFD, buffer, BUFFER_SIZE); // read data from client
    if (val < 0) {
      printf("Error in Reading...!\n");
      exit(1);
    }
    printf("Client --> %s", buffer);
    if (!strncmp("bye", buffer, 3)) {           // disconnent when read "bye"
      break;
    }

    bzero(buffer, BUFFER_SIZE);
    printf("       --> ");
    fgets(buffer, BUFFER_SIZE, stdin);          // reads a line from the terminal
    val = write(newSockFD, buffer, BUFFER_SIZE);// write data to client
    if (val < 0) {
      printf("Error in Writing...!\n");
      exit(1);
    }

    if (!strncmp("bye", buffer, 3)) {           // disconnent when write "bye"
      break;
    }
  }

  close(newSockFD);
  close(sockFD);

  return 0;
}
