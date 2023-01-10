#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <time.h> 

#define MAX_WEIGHT 5
#define MAX_BAG_ITEMS 25
#define MAX_CART_ITEMS 100  

struct GroceryItem { //grocery items
	char* name;
	float cost;
	float weight;
	int frozen;
};

struct Bag { //bags that go into the shopping carts
	struct GroceryItem** items;
	int count;
	float weight;
};

struct Cart { //shopping cart
	struct GroceryItem** items;
	int total_items;
	struct Bag** bags;
	int total_bags;
};

int addItem(struct GroceryItem* item, struct Cart* cart) { //function that adds a single item to a shopping cart
	int index = cart->total_items; //the index of the new item in the items array in the shopping cart
	cart->total_items++;
	cart->items = (struct GroceryItem **) realloc(cart->items,sizeof(struct GroceryItem)*cart->total_items);
	if(cart->items == NULL) {
		exit(0);
	}
	cart->items[index] = item; 
	return 1;
}

int findIndexbyName(struct GroceryItem* item,struct Cart* cart) { //helper function that compares the name to find the index
	int index = 0;
	for(int i =0;i<cart->total_items;i++) {
		if(strcmp(cart->items[i]->name,item->name) == 0) {
			index = i;
			return index;	
		}
	}
	return -1;
}

int findIndexbyNameBag(struct GroceryItem* item,struct Bag* bag) {//same type of helper function but using a bag instead of a cart
	int index = 0;
	for(int i =0;i<bag->count;i++) {
		if(strcmp(bag->items[i]->name,item->name) == 0) {
			index = i;
			return index;	
		}
	}
	return -1;
}

int removeItem(struct GroceryItem* item,struct Cart* cart) { //this function removes a single item from the shopping cart
	int index = findIndexbyName(item,cart); //calls the helper function to find the index
	if(index != -1) {
		struct GroceryItem** newlist; //creating a new list to add the updated array with the removed item into
		if(cart->total_items == 1) { //a condition for when you want to remove the final item
			cart->items = NULL;
			cart->total_items = cart->total_items - 1;
			return 1;
		}
		newlist = (struct GroceryItem**) malloc(sizeof(struct GroceryItem) * cart->total_items -1); //setting the size to be one item less 
		if(newlist == NULL) {
			exit(0);
		}
		for(int i = 0;i<index;i++) { //for loops to add the other elements without the removed item
			newlist[i] = cart->items[i];
		} 
		for(int i = index+1;i<cart->total_items;i++) {
			newlist[i-1] = cart->items[i];
		}
		free(cart->items); //freeing up this pointer as it's going to be replaced
		cart->items = newlist;
		cart->total_items = cart->total_items - 1;
		return 1;
	}
	else {
		return 0;
	}
}

int addtoBag(struct GroceryItem* item, struct Bag* bag) { //function that adds an item to a bag
	int index = bag->count;
	bag->count++;
	bag->items = (struct GroceryItem **) realloc(bag->items,sizeof(struct GroceryItem)*bag->count);
	if(bag->items == NULL) {
		exit(0);
	}
	bag->items[index] = item;
	bag->weight = bag->weight + item->weight; 
	return 1;
}

int addBagtoCart(struct Bag* bag, struct Cart* cart) { //function that adds a bag to a shopping cart
	int index = cart->total_bags;
	cart->total_bags++;
	cart->bags = (struct Bag **) realloc(cart->bags,sizeof(struct Bag)*cart->total_bags);
	if(cart->bags == NULL) {
		exit(0);
	}
	cart->bags[index] = bag; 
	return 1;
}

int removeItemfromBag(struct GroceryItem* item,struct Bag* bag) { //function that removes an item from a bag
	int index = findIndexbyNameBag(item,bag);
	if(index != -1) {
		struct GroceryItem** newlist;
		newlist = (struct GroceryItem**) malloc(sizeof(struct GroceryItem) * bag->count -1);
		if(newlist == NULL) {
			exit(0);
		}
		for(int i = 0;i<index;i++) {
			newlist[i] = bag->items[i];
		}
		for(int i = index+1;i<bag->count;i++) {
			newlist[i-1] = bag->items[i];
		}
		free(bag->items);
		bag->items = newlist;
		bag->count = bag->count - 1;
		return 1;
	}
	else {
		return 0;
	}
}

void printItem(struct GroceryItem* item) { //prints a single item into a neat format
	printf("%s weighing %.3fkg with price $%.2f\n",item->name,item->weight,item->cost);
}

float findHeaviest(struct Bag* bag) { //helper function to find the value of the heaviest item
	float heaviest = 0.0;
	for(int i =0;i<bag->count;i++) {
		if(heaviest<bag->items[i]->weight) {
			heaviest = bag->items[i]->weight;
		}
	}
	return heaviest;
}

void printBag(struct Bag* bag, int bagnum) { //prints the contents of a bag into a neat format using the help of printItem()
	float heaviest = findHeaviest(bag);
	printf("BAG %d (Total Weight = %.3fkg)\n",bagnum,bag->weight);
	for(int i = 0;i<bag->count;i++) {
		if(bag->items[i]->weight == heaviest) {
			printf("*"); //adds a star if it's equal to the value found by the previous function
		}
		printItem(bag->items[i]);
	}
}

void printCart(struct Cart* cart) { //prints the contents of the whole cart using the previous print function
	for(int i = 0;i<cart->total_items;i++) {
		printItem(cart->items[i]);
	}
	for(int i = 0;i<cart->total_bags;i++) {
		printBag(cart->bags[i],i+1);
	}
}

void removePerishable(struct Cart* cart) { //removes items that need to be frozen from the shopping cart
	for(int i = 0; i<cart->total_items;i++) { //this removes it if they're loose items (not in a bag)
		if(cart->items[i]->frozen == 1) {
			removeItem(cart->items[i],cart);
		}
	for(int i = 0; i<cart->total_bags;i++) { //this removes them if they're in a bag
		for(int k = 0;k<cart->bags[i]->count;k++) {
			if(cart->bags[i]->items[k]->frozen == 1) {
				removeItemfromBag(cart->bags[i]->items[k],cart->bags[i]);
			}
		}
	}
	}
}

int calcNumofBags(struct Cart* cart) { // helper function used to calculate the number of bags required to fit all the grocery items into bags keeping in mind weight and item limits
	int numofBags = 1;
	float tracker = 0.0;
	int items = 0;
	for(int i = 0;i<cart->total_items;i++) {
		if(cart->items[i]->weight<MAX_WEIGHT) {
			if(cart->items[i]->weight + tracker > MAX_WEIGHT || items>MAX_BAG_ITEMS) { //creates a new bag and resets the variables keeping track of the limits
				tracker = 0.0;
				items = 0;
				numofBags++;
			}
			else {
				tracker = tracker + cart->items[i]->weight; 
				items++;
			}
		}
	}
	if(numofBags == 1 && tracker == 0.0) { //incase there is a scenario where there are no items the bags should be 0 and not 1
		numofBags = 0;
	}
	if(tracker == 0.0) { //this is to remove any empty bags
		numofBags = numofBags - 1;
	}
	return numofBags;
}

void removePackedItems(struct Cart* cart) { //helper function that removes items from the shopping cart's items list since they were moved into bags instead
	int packed = 0;
	for(int i = 0;i<cart->total_items;i++) {
		if(cart->items[i]->weight <= MAX_WEIGHT){
			packed++;
		}
	}
	struct GroceryItem* packeditems[packed];
	int index = 0;
	for(int i=0;i<cart->total_items;i++) {
		if(cart->items[i]->weight <= MAX_WEIGHT){
			packeditems[index] = cart->items[i];
			index++;
		}
	}

	for(int i=0;i<packed;i++) {
		removeItem(packeditems[i],cart);
	}
}

void packBags(struct Cart* cart) { //packs the items into bags if they fit

	int numofBags = calcNumofBags(cart);
	int index = 0;
	for(int i = 0;i<numofBags;i++) {
		struct Bag *bag;
		bag = (struct Bag *) malloc(sizeof(struct Bag));
		if(bag == NULL) {
			exit(0);
		}
		while(index<cart->total_items) {
			if(cart->items[index]->weight<=MAX_WEIGHT) {
				if(cart->items[index]->weight + bag->weight > MAX_WEIGHT || bag->count +1 > MAX_BAG_ITEMS) {
					index++;
					break;
				}	
				else {
					addtoBag(cart->items[index],bag);
				}
			}
			index++;

		}
		addBagtoCart(bag,cart);
	}
	removePackedItems(cart);
}


void freeCart(struct Cart* cart) { //cleans up the pointers in the shopping cart
	free(cart->items);
	for(int i = 0;i<cart->total_bags;i++) {
		free(cart->bags[i]->items);
		free(cart->bags[i]);
	}
	free(cart->bags);
}

int main() {
	struct GroceryItem* testItems[12];
	char *sampleItemNames[] = {"Smart-Ones Frozen Entrees", "SnackPack Pudding", "Breyers Chocolate Icecream", "Nabob Coffee", "Gold Seal Salmon", "Ocean Spray Cranberry Cocktail", "Heinz Beans Original", "Lean Ground Beef", "5-Alive Frozen Juice", "Coca-Cola 12-pack", "Toilet Paper - 48 pack", "Crate of milk"};
	float sampleItemPrices[] = {1.99, 0.99, 2.99, 3.99, 1.99, 2.99, 0.79, 4.94, 0.75, 3.49, 40.96, 12.99};
	float sampleItemWeights[] = {0.311, 0.396, 2.27, 0.326, 0.213, 2.26, 0.477, 0.75, 0.426, 5.112, 10.89f, 6.18f};
	float sampleItemPerish[] = {1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1};
	
	for(int i = 0;i<12;i++) {
		struct GroceryItem* testitem;
		testitem = (struct GroceryItem *) malloc(sizeof(struct GroceryItem));
		testitem->name = sampleItemNames[i];
		testitem->cost = sampleItemPrices[i];
		testitem->weight = sampleItemWeights[i];
		testitem->frozen = sampleItemPerish[i];
		testItems[i] = testitem;
	}
	
	struct Cart* cart;
	cart = (struct Cart *) malloc(sizeof(struct Cart));
	srand(time(0));
	
	for(int i=0;i<50;i++) {
		addItem(testItems[rand()%12],cart);
	}
	
	printf("\nCart before packing\n\n");
	printCart(cart);
	packBags(cart);
	printf("\nCart after packing\n\n");
	printCart(cart);
	removePerishable(cart);
	printf("\nCart after removing perishables\n\n");
	printCart(cart);
	
	freeCart(cart);
	free(cart);
	for(int i = 0;i<12;i++) {
		free(testItems[i]);
	}
	
}


