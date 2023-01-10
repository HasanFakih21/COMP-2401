#include <stdio.h>

void printEncodedMaze(int maze[],unsigned char size) { //takes an encoded maze and prints the walls and gaps
	for(int i = 0;i<size;i++) {
		for(int k = 0;k<size;k++) {
			unsigned int mask = 1<< size-1-k; //mask to check each bit 
			if((maze[i] & mask)>0) {
				printf("@");
			}
			else {
				printf(" ");
			}
		}
		printf("\n");
	}
	printf("\n");
}

void printEncodedMazeWithPath(int maze[], int path[], unsigned char size) { //prints the maze again but checks if there is a path to print if there's no wall
	for(int i = 0;i<size;i++) {
		for(int k = 0;k<size;k++) {
			unsigned int mask = 1<< size-1-k;
			if((maze[i] & mask)>0) {
				printf("@");
			}
			else if((path[i]&mask)>0) {
				printf(".");	
			}
			else {
			printf(" ");
			}
		}
		printf("\n");
	}
	printf("\n");
}

void encode8by8Maze(char inputMaze[8][8], int encodedMaze[8]) { //turns a 2d array of chars into an array of ints 
	for(int i = 0;i<8;i++) {
		int encodedRow = 0; //this is what one int in the array will be
		for(int k = 0;k<8;k++) {
			if(inputMaze[i][k]==1) {
				unsigned int mask = 1<< 8-1-k; //adds the 1 in the position of the bit relative to where it was in the  array
				encodedRow = encodedRow|mask;
			}
		}
		encodedMaze[i] = encodedRow;
	}
}

void merge8by8Mazes(int topLeft[8], int topRight[8],int bottomLeft[8], int bottomRight[8],int finalMaze[14]) { //combines 4 8by8 mazes 
	for(int i = 0;i<7;i++){ //this part is for the top part of the maze
		int new1 = (topLeft[i]&~1); //this removes the wall in the middle
		int new2 = topRight[i]&~(1<<7); //also removes the wall
		finalMaze[i] = new1<<6|new2; //merges the two mazes without the walls in the middle
	}
	for(int i =0;i<7;i++) { //this part is for the bottom part
		int new1 = (bottomLeft[i+1]&~1); //skips the wall on top by starting at i+1
		int new2 = bottomRight[i+1]&~(1<<7);
		finalMaze[i+7] = new1<<6|new2;
	}

}

char isPathInMaze(int encodedMaze[], int path[], unsigned char size) { //checks to see if the path can fit into the maze
	for(int i = 0;i<size;i++) {
		for(int k = 0;k<size;k++) {
			unsigned int mask = 1<< size-1-k;
			if((encodedMaze[i] & mask)>0) { //if there is a wall in the maze
				if((path[i]&mask)>0) { //but there is also a path then the path does not fit into the maze
					return 0;	
				}
			}
			
		}
	}
	return 1;
}


int main() {

	int maze[] = {-1,-2146430423,-1109928277,-1525661045,-1602496045,-1358950569,
	-1451509759,-1927115297,-1578180479,-1354977603,-1476294999,
	-2084818261,-1163749757,-1964319689,-1341614619,-1750141691,-1256060947,
	-1515522751,-1204136611,-1955867371,-1190652827,-1553272339,-1100839163,-1999963019,
	-631368865,-1882138047,-1172944163,-1412279803,-1567107339,-164346691,-2138762879,-1};                     
	int path[] = {0, 0, 0, 0, 12, 8, 56, 32, 8032, 4416, 134115648, 67354944, 67109184,
	67109312, 133169152, 1048576, 1835008, 262144, 262144, 262144, 458752,
	65536, 65536, 65536, 65536, 983040, 67633152, 67633152, 201850880,
	164102144, 259522560, 0};
	
	int path2[] = {0, 4096, 4096, 4096, 4096, 7936, 256, 256, 448, 112, 16, 28, 6, 0};
	
	char maze1[8][8] = 
	{{1,1,1,1,1,1,1,1},
	{1,0,0,0,1,0,0,1},
	{1,0,1,0,1,1,0,1},
	{1,0,1,0,0,0,0,1},
	{1,0,1,1,1,1,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,1,0,1,0,1,1},
	{1,1,1,1,1,1,1,1}};
	
	char maze2[8][8] = 
	{{1,1,1,1,1,1,1,1},
	{1,0,0,0,0,1,0,1},
	{1,1,1,1,0,1,0,1},
	{1,0,0,1,0,1,1,1},
	{1,1,0,0,0,0,0,1},
	{1,1,1,1,0,1,1,1},
	{1,0,0,0,0,1,0,1},
	{1,1,1,1,1,1,1,1}};
	
	char maze3[8][8] = 
	{{1,1,1,1,1,1,1,1},
	{1,0,1,0,0,0,1,1},
	{1,0,1,0,1,0,0,1},
	{1,0,1,0,1,0,1,1},
	{1,0,1,0,1,0,1,1},
	{1,0,1,0,1,0,1,1},
	{1,0,0,0,1,0,0,1},
	{1,1,1,1,1,1,1,1}};
	
	char maze4[8][8] = 
	{{1,1,1,1,1,1,1,1},
	{1,0,1,0,1,0,1,1},
	{1,0,1,0,0,0,0,1},
	{1,0,0,0,1,1,1,1},
	{1,1,1,0,1,0,0,1},
	{1,0,0,0,0,0,1,1},
	{1,1,0,1,1,0,0,1},
	{1,1,1,1,1,1,1,1}};
	
	int encodedMaze1[8];
	int encodedMaze2[8];
	int encodedMaze3[8];
	int encodedMaze4[8];
	
  	printEncodedMaze(maze,32);
  	printEncodedMazeWithPath(maze,path,32);
  	if(isPathInMaze(maze,path,32)) {
  		printf("The path fits\n\n");
  	}
  	encode8by8Maze(maze1, encodedMaze1);
  	encode8by8Maze(maze2, encodedMaze2);
  	encode8by8Maze(maze3, encodedMaze3);
  	encode8by8Maze(maze4, encodedMaze4);
  	printEncodedMaze(encodedMaze1,8);
  	printEncodedMaze(encodedMaze2,8);
  	printEncodedMaze(encodedMaze3,8);
  	printEncodedMaze(encodedMaze4,8);
  	
  	int finalMaze[14] = {};
  	
  	merge8by8Mazes(encodedMaze1, encodedMaze2,encodedMaze3,encodedMaze4,finalMaze);
  	printEncodedMaze(finalMaze,14);
  	
  	int* encodedMazes[4] = {encodedMaze1,encodedMaze2,encodedMaze3,encodedMaze4}; //array for pointers of int array to do the permutation 
  	
  	for(int i = 0;i<4;i++) { //shows all 256 possible combinations of the 14x14 maze combination
  		for(int k = 0;k<4;k++) {
  			for(int j = 0;j<4;j++) {
  				for(int l = 0;l<4;l++) {
  					merge8by8Mazes(encodedMazes[i], encodedMazes[k],encodedMazes[j],encodedMazes[l],finalMaze);
  					printEncodedMaze(finalMaze,14);
  					printf("%d , %d, %d, %d\n",i+1,k+1,j+1,l+1);
  					if(isPathInMaze(finalMaze,path2,14)) {
  						printf("The path fits\n\n");
  					}
  				}
  			}
  		}
  	}
}


