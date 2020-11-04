#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
#include <string.h>
/*
struct meta:
	stores the size value of the meta data, used to tell the size of the space stored and if free or not
	-> If the size is negative then the space is free and the number is the ammount of space
	->If positive the space has been allocated in the array and the size is the number
*/
typedef struct meta {
	short int size;
} Metadata;

static char Memory[MEMSIZE] = {0, 0}; 
//static Metadata* Initptr;
/*
myfree(void* addr, char * filename, int line):
This function takes in a address pointer, a filename, and line number. Given an address to a space in the 
memory array, the free function attempts to free the space while also checking for contiguous free space 
and out of bounds errors. The free function checks to see if the size value stored in the metadata is negative or 
not, if negative then it is  known that the value is already freed, if not it will free the space. The free function
iterates to each metadata section  finding the specific space needed to free.
*/

void myfree(void* addr, char* filename, int line){
	if(DEBUG){
		printf("Running!\n");
		printf("Memory: %p ; Addr: %p\n", Memory, addr);
		printf("First Condition: %ld\n", (Memory + MEMSIZE) - (char*)addr);
		printf("Second Condition: %ld\n", (char*)addr - Memory);
		printf("Full Condition Result: %d\n", (Memory + MEMSIZE) - (char*)addr < 0 || (char*)addr - Memory < 0);
	}
	//check if the pointer is in our range of Memory
	if ((Memory + MEMSIZE) - (char*)addr < 0 || (char*)addr - Memory < 0){
		printf("%s: %d Pointer out of range of Memory. \n", filename, line);
		exit(EXIT_FAILURE);
	}
	int currentLocation = 0;
	Metadata* cur = (Metadata*) (Memory + currentLocation);
	int nextLocation = cur->size + sizeof(Metadata);
	if (cur->size < 0){
		nextLocation = -1*cur->size + sizeof(Metadata);
	}
	Metadata *prev = NULL, *next = (Metadata*) (Memory + nextLocation);
	char* nextData = (char*) (Memory + nextLocation + sizeof(Metadata)), *curData = (char*) (Memory + currentLocation + sizeof(Metadata));
	int negCur = 1, negNext = 1;
	while(currentLocation < MEMSIZE && nextData <= (char*) addr){
		if(cur->size < 0){
			negCur = -1;
		}
		if (next->size < 0){
			negNext = -1;
		}
		currentLocation += negCur*cur->size + sizeof(Metadata);
		nextLocation += negNext*next->size + sizeof(Metadata);
		prev = cur;
		cur = (Metadata*) (Memory + currentLocation);
		next = (Metadata*) (Memory + nextLocation);
		nextData = (char*) (Memory + nextLocation + sizeof(Metadata));
		curData = (char*) (Memory + currentLocation + sizeof(Metadata));
		negCur = 1;
		negNext = 1;
		if(DEBUG){
			printf("PREV: %p, CURRENT: %p, NEXT: %p\n", prev, cur, next);
			printf("NEXT DATA: %p, CURRENT DATA: %p\n", nextData, curData);
		}
	}
	if(cur->size < 0){
		printf("%s: %d Can not free an already freed block of memory. \n", filename, line);
		exit(EXIT_FAILURE);
	}
	if (curData != (char*)addr){
		printf("%s: %d Pointer is not starting at the beginning of the allocated block. \n", filename, line);
		exit(EXIT_FAILURE);
	}
	cur->size = -1*cur->size;
	if (next != NULL){
		if (next->size < 0){
			cur->size = cur->size + next->size - sizeof(Metadata);
		}
	}
	if (prev != NULL){
		if (prev->size < 0){
			prev->size = prev->size + cur->size - sizeof(Metadata);
		}
	}	

}
/*
mymalloc(int size, char* filename, int line):
This function takes in a intger size value, a filenmame, and a intger line value. This function takes in a 
given size of data and allocates space in memory for the given chunk. The malloc checks to see if there is 
a chunk of space of size in the array to allocate, if there is it allocates the first space, if not it responds 
accordingly.
*/
void* mymalloc(int size, char* filename, int line){
	

	if (size < 0){
		printf("%s:%d Can not allocate memory of of negative size.\n", filename, line);
		exit(EXIT_FAILURE);
	}
	if (size == 0){
		return NULL;
	}

	Metadata* initTest = (Metadata*)Memory;
	if (initTest->size == 0){ //can't allocate 0 bytes so the only case when the first metadata is 0 is when it's the first time
		int memsize = MEMSIZE;
		initTest->size = -1*memsize + sizeof(Metadata);
	}
	short int currentLocation = 0, temp = 0, largestFreeSpace;
	Metadata* cur = (Metadata*)Memory; //making a pointer to the first piece of metadata;
	if(DEBUG){	printf("SEARCHING FOR BLOCK\n"); }
	while (currentLocation < MEMSIZE){
		if(DEBUG){ printf("LOOKING AT LOCATION %d\n", currentLocation); } 
		cur = (Metadata*) (Memory + currentLocation);
		if(DEBUG){ printf("CURRENT SIZE: %d\n", cur->size); } 
		if (cur->size * -1 >= size){
			temp = cur->size * -1;
			cur->size = size;
			if (temp - size >= 3) { //this means we have to put in a footer pointer
				Metadata* footer = (Metadata*) (Memory + currentLocation + size + sizeof(Metadata)); 
				footer->size = (temp - size - 2)*-1;
			}
			else if (temp > size){
				cur->size += temp-size;
			}
			return (void*) (Memory + currentLocation + 2);
		}
		if (cur->size < largestFreeSpace){
			largestFreeSpace = cur->size;

		}
		currentLocation += cur->size + sizeof(Metadata);
	}
	printf("%s:%d Not enough space in free memory. The largest block available is %d bytes long\n", filename, line, largestFreeSpace);
	return NULL;
}

//use to visually represent what is happening
/*
void printMemory(){
	int currentLocation = 0;
	Metadata* current = (Metadata*) (Memory + currentLocation); 
	int neg = 1;
	printf("---------------------MEMORY------------------------\n");
	while(currentLocation < MEMSIZE){
		printf("Metadata\n");
		printf("Block Size: %d\n", current->size);
		printf("Current Address: %p\n", Memory + currentLocation);
		printf("V\n");
		if (current->size < 0){
			neg = -1;
		}
		currentLocation += neg*current->size + sizeof(Metadata);
		current = (Metadata*) (Memory + currentLocation);
		neg = 1;
	}
	printf("---------------------END---------------------------\n");
}*/
