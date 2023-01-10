#include <stdio.h>
#define xsize 5 /*number of rows for each box*/
#define ysize 5 /*number of coulmns for each box*/
char box[xsize][ysize]; 
int numofpackages = 24;
char o = 'o'; /*o is used to indicate an empty spot in the array*/
int smallp; /*declaring the packages*/
int mediump;
int largep;

int initializeArray() { /*this function is used to reset the array*/
 for(int x = 0;x<xsize;++x) {
  	for(int y = 0;y<ysize;++y) {
  	    box[x][y] = o;
  	}
  }
  return 0;
}

int printReceipt(int small, int medium, int large, int boxes) { /*takes the random numbers generated for each package size and the total boxes used to store them all and prints the receipt*/
  float largecost = 19.99*large;
  float mediumcost = 10.99*medium;
  float smallcost = 5.99*small;
  float boxcost = 1.49*boxes;
  float total = smallcost+mediumcost+largecost;
  float taxes = (smallcost+mediumcost+largecost)*0.13;
  float overalltotal = total+taxes+boxcost;
  printf("Order: Large = %d, Medium = %d, Small = %d\n",large,medium,small);
  printf("Small (%2d%s        $%.2f\n",small," units)",smallcost);
  printf("Medium (%2d%s       $%.2f\n",medium," units)",mediumcost);
  printf("Large (%2d%s        $%.2f\n",large," units)",largecost);
  printf("-----------------------------\n");
  printf("Total\t\t\t$%.2f\n",total);
  printf("Shipping (%d boxes)\t$%.2f\n",boxes, boxcost);
  printf("HST (13%)\t\t$%.2f\n",taxes);
  printf("-----------------------------\n");
  printf("Amount Charged\t\t$%.2f\n",overalltotal);
  return 0;
}

int printBox() { /*prints the boxes and shows how each package is placed*/
   for(int x = 0;x<xsize;++x) {
  	for(int y = 0;y<ysize;++y) {
  	if(box[x][y]=='o') {
  	  printf(" ");
  	}
  	else {
  	  printf("%c",box[x][y]);
  	}
  	  }
  	  printf("\n");
  	}
  return 0;
}

int generateBox() { /*tries to fit in each package into the box*/
 for(int x = 0;x<xsize;++x) {
  	for(int y = 0;y<ysize;++y) {
  	  if(box[x][y]=='o') {
  	    if(x!=xsize-1 && y!=ysize-1 && largep>0) { /*this makes sure that if there is a 2x2 area available to check*/
  	      if((box[x+1][y] == 'o')&&(box[x][y+1] == 'o')&&(box[x+1][y+1] == 'o')) { /*this makes sure that it is infact empty*/
  	        box[x][y]='L'; /*if it is empty the 2x2 area is filled with the character L to indicate a large package takes that slot*/
  	        box[x+1][y] = 'L';
  	        box[x][y+1] = 'L';
  	        box[x+1][y+1] = 'L';
  	        largep = largep - 1;
  	      }
  	    }
  	    else if(y!=ysize-1 && mediump>0) {
  	     if(box[x][y+1]=='o') {
  	      box[x][y] = 'M';
  	      box[x][y+1] = 'M';
  	      mediump = mediump - 1;
  	     }
  	    }
  	    else if(x!=xsize-1 && mediump>0) {
  	      if(box[x+1][y]=='o') {
  	      box[x][y] = 'M';
  	      box[x+1][y] = 'M';
  	      mediump = mediump - 1;
  	    }
  	  }
  	   else {
  	    if(smallp>0) {
  	      box[x][y] = 'S';
  	      smallp = smallp - 1;
  	    }
  	   }
  	  }  
  	}
}
return 0;
}

int main() {

  initializeArray();
  srand(time(0));
  smallp = (rand() % (numofpackages + 1));
  mediump = (rand() % ((numofpackages - smallp) + 1));
  largep = numofpackages - smallp - mediump;
  int orders = smallp;
  int orderm = mediump;
  int orderl = largep;
  printf("random numbers are, %d %d %d\n",smallp,mediump,largep);
  int numofboxes = 0;
  while((smallp+mediump+largep)>0) { 
    numofboxes = numofboxes + 1;
    initializeArray();
    generateBox();
    printBox();
    printf("\n");
  }
  
  printReceipt(orders,orderm,orderl,numofboxes);
  return 0;	
}
