#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "simulator.h"


int main() {
  // ... ADD SOME VARIABLES HERE ... //
  int                 clientSocket, addrSiz;
  struct sockaddr_in  clientAddr;
  char signal = STOP;
  // Register with the server
  clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (clientSocket < 0) {
    printf("*** CLIENT ERROR: Could not open socket.\n");
    exit(-1);
  }
  // ... WRITE SOME CODE HERE ... //
  memset(&clientAddr, 0, sizeof(clientAddr));
  clientAddr.sin_family = AF_INET;
  clientAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
  clientAddr.sin_port = htons((unsigned short) SERVER_PORT);
  // Send command string to server
  sendto(clientSocket, &signal, 1 , 0, (struct sockaddr *) &clientAddr, sizeof(clientAddr));
  // ... WRITE SOME CODE HERE ... //
  close(clientSocket);
}
