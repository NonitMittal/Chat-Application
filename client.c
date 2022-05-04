#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT "1030"
#define BUFFER_SIZE 1024
#define MAXDATASIZE 100

int main(int argc, char * argv[]) {

  int sockFD, portNo, val;
  struct sockaddr_in servAddr;
  char buffer[BUFFER_SIZE];

  // if server ip address is provided or not
  if (argc < 2) {
    printf("Provide required command-line inputs as: <filename.c> <server_IP_addr>\n");
    exit(1);
  }

  portNo = atoi(PORT);      // convert string to its equivalent int.
  sockFD = socket(AF_INET, SOCK_STREAM, 0);
                            // create socket (IPv4, TCP/Stream Socket, IP protocol)
                            // sockFD = socket file descriptor

  if (sockFD < 0) {
    printf("Error in opening socket\n");
    exit(1);
  }

  servAddr.sin_family = AF_INET;                  // address family 
  servAddr.sin_port = htons(portNo);              // host to network short
  servAddr.sin_addr.s_addr = inet_addr(argv[1]);  // server address

  // try to connect with the server
  if (connect(sockFD, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
    printf("Connection failed...!\n");
    exit(1);
  }

  while (1) {
    bzero(buffer, BUFFER_SIZE);                   // clear the buffer content
    printf("       --> ");
    fgets(buffer, BUFFER_SIZE, stdin);            // reads a line from the terminal
    val = write(sockFD, buffer, BUFFER_SIZE);     // write data to server
    if (val < 0) {
      printf("Error in Writing...!\n");
      exit(1);
    }
    if (!strncmp("bye", buffer, 3)) {             // disconnent when write "bye"
          break;
    }

    bzero(buffer, BUFFER_SIZE);
    val = read(sockFD, buffer, BUFFER_SIZE);      // read data from server
    if (val < 0) {
      printf("Error in Reading...!\n");
      exit(1);
    }
    printf("Server --> %s", buffer);

    if (!strncmp("bye", buffer, 3)) {             // disconnent when read "bye"
      break;
    }
  }

  close(sockFD);
  return 0;
}
