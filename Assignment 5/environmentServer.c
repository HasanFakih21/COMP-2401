#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "simulator.h"


Environment    environment;  // The environment that contains all the robots
sem_t   serverBusyIndicator;



// Handle client requests coming in through the server socket.  This code should run
// indefinitiely.  It should repeatedly grab an incoming messages and process them. 
// The requests that may be handled are STOP, REGISTER, CHECK_COLLISION and STATUS_UPDATE.   
// Upon receiving a STOP request, the server should get ready to shut down but must
// first wait until all robot clients have been informed of the shutdown.   Then it 
// should exit gracefully.  
void *handleIncomingRequests(void *e) {
	char   online = 1;
  	// ... ADD SOME VARIABLE HERE... //
	int                 serverSocket;
  	struct sockaddr_in  serverAddr, clientAddr;
  	int                 status, addrSize, bytesReceived;
  	fd_set              readfds, writefds;
  	unsigned char                buffer[10];
  	// Initialize the server
	serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  	if (serverSocket < 0) {
   		printf("*** SERVER ERROR: Could not open socket.\n");
    		exit(-1);
 	}
 	memset(&serverAddr, 0, sizeof(serverAddr)); 
  	serverAddr.sin_family = AF_INET;
  	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  	serverAddr.sin_port = htons((unsigned short) SERVER_PORT);
  	
  	status = bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
  	if (status < 0) {
    		printf("*** SERVER ERROR: Could not bind socket.\n");
   		exit(-1);
  	}
  	// Wait for clients now
  	char flag = 0;
	while (online) {
		// ... WRITE YOUR CODE HERE ... //
		FD_ZERO(&readfds);
    		FD_SET(serverSocket, &readfds);
    		FD_ZERO(&writefds);
    		FD_SET(serverSocket, &writefds);
    		status = select(FD_SETSIZE, &readfds, &writefds, NULL, NULL);
    		if (status == 0) {
      			// Timeout occurred, no client ready
    		}
    		else if (status < 0) {
      			printf("*** SERVER ERROR: Could not select socket.\n");
      			exit(-1);
   		}
    		else {
    			addrSize = sizeof(clientAddr);
    			bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr *) &clientAddr, &addrSize);
    			if(buffer[0] == REGISTER) {
    				if(((Environment *)e)->numRobots == MAX_ROBOTS) {
    					char signal = NOT_OK;
    					sendto(serverSocket, &signal, 1 , 0, (struct sockaddr *) &clientAddr, sizeof(clientAddr));
    				}
    				else {
    					char signal = OK;
    					char id = ((Environment *)e)->numRobots;
    					((Environment *)e)->numRobots++;
    					int minimum_number = ROBOT_RADIUS;
    					int max_number = ENV_SIZE - ROBOT_RADIUS;
    					srand(time(0));
    					float x = rand() % (max_number + 1 - minimum_number) + minimum_number; //creates random number from 15 to 585
    					float y = rand() % (max_number + 1 - minimum_number) + minimum_number;
    					int direction = rand() % (180 + 1);
    					char sign = rand() % (1 + 1);
    					//printf("X:      %.1f,Y:        %.1f\n",x,y);
    					unsigned char x1,x2,y1,y2;
    					if(x>255) { //splits float into two byte sized integers
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
    					//printf("ORIGINAL X: %d %d Y: %d %d\n",x1,x2,y1,y2);
    					char response[8] = {signal,id,x1,x2,y1,y2,direction,sign};
    					//printf("before we send: %d %d Y: %d %d\n",x1,x2,y1,y2);
    					sendto(serverSocket, response, sizeof(response) , 0, (struct sockaddr *) &clientAddr, sizeof(clientAddr));
    					Robot robot;
    					robot.x = x;
    					robot.y = y;
    					if(sign == 0) {
    						robot.direction = direction;
    					}
    					else {
    						robot.direction = -direction;
    					}
    					((Environment *)e)->robots[id] = robot; //adds the new registered robot to the environment
    				}
    			}
    			
    			if(buffer[0] == CHECK_COLLISION) {
    				char signal;
    				float x = buffer[2]*256 + buffer[3]; //converts the two byte sized integers back
  				float y = buffer[4]*256 + buffer[5];
  				int direction;
  				if(buffer[7] == 0) {
  					direction = buffer[6];
 				}
 				else {
  					direction = -buffer[6];
  				}
  				char id = buffer[1];
  				float newx = x + ROBOT_SPEED * cos(direction);
    				float newy = y + ROBOT_SPEED * sin(direction);
    				float minimum_number = ROBOT_RADIUS;
    				float max_number = ENV_SIZE - ROBOT_RADIUS;
  				if( (newx<=minimum_number) || (newx>=max_number) || (newy<=minimum_number) || (newy>=max_number) ) { //checks to see if the current trajectory will send the object outside of the bounds set
  					//printf("X: %.1f Y: %.1f newX: %.1f newY: %.1f Direction: %d\n",x,y,newx,newy,direction);
  					signal = NOT_OK_BOUNDARY;
  				}
  				else {
  					signal = OK;
  				}
  				for(int i = 0;i<((Environment *)e)->numRobots;i++) { //loops to compare every robots position to see if the trajectories will collide
  					if(i != id) {
  						float otherx = ((Environment *)e)->robots[i].x;
  						float othery = ((Environment *)e)->robots[i].y;
  						float otherdirection = ((Environment *)e)->robots[i].direction;
  						float newotherx = otherx + ROBOT_SPEED * cos(otherdirection);
  						float newothery = othery + ROBOT_SPEED * sin(otherdirection);
  						if((newx>newotherx-ROBOT_RADIUS && newx<newotherx+ROBOT_RADIUS)&&(newy>newothery-ROBOT_RADIUS && newy<newothery+ROBOT_RADIUS)) {
  							signal = NOT_OK_COLLIDE;
  						}
  					}
  				}
  				if(flag != 0) { //flag turns to 1 when stop command is sent
  					flag++;
    					signal = LOST_CONTACT;
    				}
  				sendto(serverSocket, &signal, sizeof(signal) , 0, (struct sockaddr *) &clientAddr, sizeof(clientAddr));
    			}
    			
    			
    			if(buffer[0] == STATUS_UPDATE) {
    				int id = buffer[1];
  				float x = buffer[2]*256 + buffer[3];
				float y = buffer[4]*256 + buffer[5];
  				int direction;
  				if(buffer[7] == 0) {
  					direction = buffer[6];
 				}
 				else {
  					direction = -buffer[6];
  				}
  		
  				((Environment *)e)->robots[id].x = x;
  				((Environment *)e)->robots[id].y = y;
  				((Environment *)e)->robots[id].direction = direction; //updates the values of the robot
    			}
    			
    			if(flag>=((Environment *)e)->numRobots) { //checks if every robot has received a no signal response then shuts down the server
    				((Environment *)e)->shutDown = 1;
    				break;
    			}
    			
     			if(buffer[0] == STOP) {
     				flag = 1;
    			}
  		}
	}
  	// ... WRITE ANY CLEANUP CODE HERE ... //
  	close(serverSocket);
  	printf("SERVER: Shutting down.\n");
  	pthread_exit(NULL);
}




int main() {
	pthread_t    serverThread;
  	pthread_t    draw;
	// So far, the environment is NOT shut down
	environment.shutDown = 0;
  
	// Set up the random seed
	srand(time(NULL));
	
	sem_init(&serverBusyIndicator, 0, 1);
	// Spawn an infinite loop to handle incoming requests and update the display
	pthread_create(&serverThread, NULL, handleIncomingRequests, &environment);
	pthread_create(&draw, NULL, redraw, &environment);
	// Wait for the update and draw threads to complete
	pthread_join(draw, NULL);
	pthread_join(serverThread, NULL);
}
