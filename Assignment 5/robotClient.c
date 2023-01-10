#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "simulator.h"




// This is the main function that simulates the "life" of the robot
// The code will exit whenever the robot fails to communicate with the server
int main() {
  // ... ADD SOME VARIABLE HERE ... //
  int                 clientSocket, addrSize, bytesReceived;
  struct sockaddr_in  clientAddr;
  unsigned char                buffer[10];   // stores sent and received data
  char                signal = REGISTER;
  // Set up the random seed
  srand(time(NULL));
  
  // Register with the server
  clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (clientSocket < 0) {
    printf("*** CLIENT ERROR: Could not open socket.\n");
    exit(-1);
  }
  
  memset(&clientAddr, 0, sizeof(clientAddr));
  clientAddr.sin_family = AF_INET;
  clientAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
  clientAddr.sin_port = htons((unsigned short) SERVER_PORT);
  addrSize = sizeof(clientAddr);
  // Send register command to server.  Get back response data
  sendto(clientSocket, &signal , 1 , 0, (struct sockaddr *) &clientAddr, sizeof(clientAddr));
  bytesReceived = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (struct sockaddr *) &clientAddr, &addrSize);
  for(int i = 0;i++;i<bytesReceived) {
  	buffer[i] = (int)(buffer[i]);
  }
 // printf("BEFORE MATH checking X:-%d %d Y:-%d %d\n",buffer[2],buffer[3],buffer[4],buffer[5]);
  // and store it.   If denied registration, then quit.
  if(buffer[0] == NOT_OK) {
  	printf("ERROR: Registration Failed");
  	exit(0);
  }
  signal = CHECK_COLLISION;
  int id = buffer[1];
  float x = buffer[2]*256 + buffer[3];
  float y = buffer[4]*256 + buffer[5];
  //printf("checking X: %.1f -%d %d Y: %.1f -%d %d\n",x,buffer[2],buffer[3],y,buffer[4],buffer[5]);
  int direction;
  if(buffer[7] == 0) {
  	direction = buffer[6];
  }
  else {
  	direction = -buffer[6];
  }
  srand(time(0));
  char leftOrRight = rand() % 2; //random number between 1 and 0 to determine the direction a robot will rotate by
  char flag = 0; //keeps track of how many consecutive rotations are happening
  // Go into an infinite loop exhibiting the robot behavior
  while (1) {
    char x1,x2,y1,y2; 
    if(x>255) {
    	x1 = x/256;
    	x2 = (int)x%256;
    }
    else {
    	x1 = 0;
    	x2 = x;
    }
    					
   if(y>255) {
    	y1 = y/256;
    	y2 = (int)y%256;
    }
    else {
    	y1 = 0;
    	y2 = y;
    }
    char checklist[8] = {signal,id,x1,x2,y1,y2,direction,(direction<0)?(1):(0)}; 
    // Check if can move forward
      //printf("X1:      %d,Y1:        %d y: %.1f typecast: %d\n",checklist[3],checklist[5],y,(int)y%256);
    sendto(clientSocket, checklist , sizeof(checklist) , 0, (struct sockaddr *) &clientAddr, sizeof(clientAddr)); //sends check collision command to the server
    // Get response from server.
    bytesReceived = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (struct sockaddr *) &clientAddr, &addrSize);
    // If ok, move forward
    
    if(buffer[0] == LOST_CONTACT) { //server is shutting down
    	printf("ROBOT %d shutting down\n",id+1);
    	close(clientSocket);
  	exit(0);
    }
 
    else if(buffer[0] == OK) { //no collision expected 
    	x = x + ROBOT_SPEED * cos(direction);
    	y = y + ROBOT_SPEED * sin(direction);
    	flag = 0;
    }
    // Otherwise, we could not move forward, so make a turn.
    else if(buffer[0] == NOT_OK_BOUNDARY ||  buffer[0] == NOT_OK_COLLIDE) { //if collision is expected then a rotation to the left or right picked randomly occurs
    	if(flag == 0) {
    		leftOrRight = rand() % 2; //if it didn't rotate the last loop it'll do a random direction
    	}
    	flag++;
    	if(flag>24) { //if it has been rotating for more than 24 times it's safe to assume the robot is stuck and it'll be given a new location helps unstuck robots spawning inside each other
    		x = rand() % (ENV_SIZE - ROBOT_RADIUS + 1 - ROBOT_RADIUS) + ROBOT_RADIUS;
    		y = rand() % (ENV_SIZE - ROBOT_RADIUS + 1 - ROBOT_RADIUS) + ROBOT_RADIUS;
    	}
    	if(leftOrRight) {
    		if(direction + ROBOT_TURN_ANGLE > 180) {
    			direction = ((direction + ROBOT_TURN_ANGLE) - 180) + -180;
    		}
    		else {
    			direction = direction + ROBOT_TURN_ANGLE;
    		}
    	}
    	else {
    		if(direction - ROBOT_TURN_ANGLE < -180) {
    			direction = ((direction - ROBOT_TURN_ANGLE) + 180); 
    		}
    		else {
    			direction = direction - ROBOT_TURN_ANGLE;
    		}
    	}
    	//printf("inside client X: %.1f Y: %.1f Direction: %d\n",x,y,direction);
    }
    // If we were turning from the last time we collided, keep
    // turning in the same direction, otherwise choose a random 
    // direction to start turning.
    // Send update to server
   // printf("X: %.1f Y: %.1f Direction: %d\n",x,y,direction);
    char update[8] = {STATUS_UPDATE,id,(x>255)?(x/256):(0),(x>255)?((int)x%256):(x),(y>255)?(y/256):(0),(y>255)?((int)y%256):(y),direction,(direction<0)?(1):(0)};
    sendto(clientSocket, update , sizeof(update) , 0, (struct sockaddr *) &clientAddr, sizeof(clientAddr));
    // Uncomment line below to slow things down a bit 
    usleep(10000);
  }
}

