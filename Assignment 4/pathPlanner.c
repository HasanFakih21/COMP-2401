#include <stdio.h>
#include <stdlib.h>

#include "obstacles.h"
#include "display.h"



int createVertices(Environment *environment) { //creates the vertices based on every corner of every obstacle
	environment->numVertices = environment->numObstacles * 4; 
	environment->vertices = (Vertex *)malloc(environment->numVertices*sizeof(Vertex)); 
	if (environment->vertices == NULL) {
		printf("Memory allocation error\n");
		exit(0); 
	}
	int index = 0; //used to keep track of the vertices index as the for loop goes
	for(int i = 0;i<environment->numObstacles;i++) {
		environment->vertices[index].x = environment->obstacles[i].x; //sets the 4 points for each obstacle at each index in the vertices array
		environment->vertices[index].y = environment->obstacles[i].y;
		index++;
		environment->vertices[index].x = environment->obstacles[i].x + environment->obstacles[i].w;
		environment->vertices[index].y = environment->obstacles[i].y;
		index++;
		environment->vertices[index].x = environment->obstacles[i].x;
		environment->vertices[index].y = environment->obstacles[i].y - environment->obstacles[i].h;
		index++;
		environment->vertices[index].x = environment->obstacles[i].x + environment->obstacles[i].w;
		environment->vertices[index].y = environment->obstacles[i].y - environment->obstacles[i].h;
		index++;
		environment->vertices[index].obstacle = &environment->obstacles[i];
	}
	return environment->numVertices;
}

void createNeighbours(Vertex *vertex, Environment *environment) { //helps set the neighbours for each vertex which is just every other vertex excluding itself
	if(environment->numVertices != 0) {
		Neighbour *head = (struct Neighbour *) malloc(sizeof(Neighbour)); 
 		if (head == NULL) {
			printf("Memory allocation error\n");
			exit(0); 
		}
		int start; //this variable helps initialize the head pointer with a useful value
		if(vertex != &(environment->vertices[0])) {
			head->vertex = &environment->vertices[0]; //if the first vertex isn't equal to the vertex for which we're trying to create the neighbors then it'll set the head to point at the first value in the vertices array
			start = 1; //start set to 1 because vertices[0] is now the head so there is no need to start at 0 in the upcoming loop 
		}	
		else {
			head->vertex = &environment->vertices[1];
			start = 2; //start at 2 if the first is equal because we can ignore the first value and the second value is now the head
		}
		head->next = NULL;
		Neighbour *newNeighbour;
		Neighbour *tail = head; //useful to set vertex->lastNeighbour later
		for(int i = start;i<environment->numVertices;i++) {
			if(vertex != &environment->vertices[i]) { //prevents it from adding itself as a neighbour
				newNeighbour = (struct Neighbour *) malloc(sizeof(Neighbour));
				if (newNeighbour == NULL) {
					printf("Memory allocation error\n");
					exit(0); 
				}
				newNeighbour->vertex = &environment->vertices[i];
				newNeighbour->next = NULL;
				tail->next = newNeighbour;
				tail = newNeighbour;
			}
		}
		vertex->firstNeighbour = head;
		vertex->lastNeighbour = tail;
	}
}

int createEdges(Environment *environment) { //creates the "edges" which at the moment is just every vertex to every other vertex besides itself
	for(int i = 0;i<environment->numVertices;i++) {
		createNeighbours(&environment->vertices[i],environment); //calls the create neighbours function for every vertex in the vertices array
	}
	int totalEdges = environment->numVertices * (environment->numVertices - 1); //28*27 total number of vertices * total number of neighbours
	return totalEdges;
}

int checkIntersect(Vertex *a, Vertex *b, Environment *environment) { //this function returns a 1 if two edges intersect and 0 if they don't
	for(int i = 0;i<environment->numObstacles;i++) { //the idea is to compare one edge with every edge of every obstacle in the enviorment
		int x[4];
		int y[4];
		
		x[0] = environment->obstacles[i].x; //sets the 4 points of every corner of an obstacle
		y[0] = environment->obstacles[i].y;
	
		x[1] = environment->obstacles[i].x + environment->obstacles[i].w;
		y[1] = environment->obstacles[i].y;

		x[2] = environment->obstacles[i].x;
		y[2] = environment->obstacles[i].y - environment->obstacles[i].h;

		x[3] = environment->obstacles[i].x + environment->obstacles[i].w;
		y[3] = environment->obstacles[i].y - environment->obstacles[i].h; 
		
		for(int i = 0;i<4;i++) { // for loop to go through every edge on the obstacle which are xy0,xy1 xy1,xy2, xy2,xy3, xy3,xy0
			int index;
			if(i == 3) {
				index = 0;
			}
			else {
				index = i+1;
			}
			float doMath(int,int,int,int,int,int,int,int);
			float num = doMath(a->x,a->y,b->x,b->y,x[i],y[i],x[index],y[index]); //calls the do math function and sends every point there is
			if(num>0 && num<1) { //checks if they intersect
				return 1;
			}
		}
	}
	return 0;
}

float doMath(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4) { //helper function to make the formula easier 
	float denominator = (((y4-y3)*(x2-x1))-((x4-x3)*(y2-y1))); //this calculates the denominator of the formula
	float Ua = (((x4-x3)*(y1-y3)) - ((y4-y3)*(x1-x3)))/denominator; //calculates Ua
	float Ub = (((x2-x1)*(y1-y3)) - ((y2-y1)*(x1-x3)))/denominator; //calculates Ub
	float final = Ua+Ub/2; //takes the average and returns it
	return final;
}

int removeEdges(Environment *environment) { //this function was intended to remove the edges that intersect with an obstacles edge currently leads to a segmentation fault
	/*for (int i=0; i<environment->numVertices; i++) { //loops through every vertex
		Neighbour *n = environment->vertices[i].firstNeighbour; //takes the inital neighbour of the vertex
		while (n != NULL) {
	  		if(checkIntersect(&environment->vertices[i],n->vertex,environment)) { //checks if the current neighbour vertex edge intersects with any obstacle's edge
	  			removeNeighbour(n,environment->vertices[i].firstNeighbour,environment->vertices[i]); //if it does intersect it calls the removeNeighbour function to remove the neighbour from the linked list 
	  			n =  environment->vertices[i].firstNeighbour; //resets the n pointer so it traverses the full list again until it finds the next intersection 
	  			continue;
	  		}
	  		if(n != NULL) {
	  			n = n->next; //if there was no intersection it moves on to the next neighbour and checks that, it'll keep going until it goes through the whole neighbours list without finding an intersection
	  		}
		}	
    }	*/
}

void debugPrint(Neighbour *head) { //function used for debugging
	int count = 0;
	while (head != NULL) {
		count++;
	  	printf("%d,%d\n",head->vertex->x,head->vertex->y);
	  	head = head->next;
	}
	printf("there are %d neighbours\n",count);
}

void removeNeighbour(Neighbour *rm,Neighbour *head,Vertex *vertex) { //this function causes a segmentation fault, it was intended to remove a neighbour from the linked list, it takes the neighbour to be removed, the head of the neighbours list, and the vertex that it belongs to 
	if(head == rm) { //checks that if the item being removed is the head of the list
		vertex->firstNeighbour = head->next; //if it is it will set the firstneighbour to be the next item
		free(rm); //frees the inital first neighbour as it's no longer in use
		return;
	}
	while (head != NULL) {
	  	if(rm == head->next) { //checks if the next item is the one to be removed, if it is the loop will end with the pointer pointing to the item before it
	  		break;
	  	}
	  	head = head->next;
	}
	if(vertex->lastNeighbour == rm) { //if the item to be removed is the tail of the list it will set the item before it as the new tail
		head->next = NULL;
		vertex->lastNeighbour = head;
	}
	head->next = rm->next; //sets the next of the item before the item to be removed as the next of the item to be removed, so the previous is linked to the next 
	free(rm); //frees the pointer since it's no longer in use
}

void cleanupEverything(Environment *environment) { //function that frees all allocated pointers
	for(int i = 0;i<environment->numVertices;i++) { //goes through every vertex
		Neighbour *n = environment->vertices[i].firstNeighbour; //creates a new pointer to go through the linked list of neighbours
		Neighbour *next = n->next; //a next pointer so when the n pointer is freed the position is not lost
		while (n != NULL) {
			next = n->next;
			free(n);
	  		n = next;
		}
	}
	free(environment->vertices); //frees the memory allocated for the vertices array
	free(environment->obstacles); //frees the memory allocated for the obstacles array
}

