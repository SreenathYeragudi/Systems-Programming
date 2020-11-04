

Description:
The design behind our mymalloc.c was to create a malloc and free that could efficiently replicate the actions of malloc and free following the given conditions. 
Using 2 bytes of metadata, our algorithm minimizes space and maximizes efficiency when allocating memory in the array. The metadata is a short int that stores the size of the block allocated and or free. 
Given an int size to mymalloc(), allocates the space plus metadata in the global memory array and then is freed by giving an address pointer to myfree(). The data stored in the metadata helps with identifying the free blocks in the array. 
If the metadata is positive then the space has been allocated by malloc() and can be freed, else if the metadata is negative, the data has been freed and is waiting to be allocated. 
Our program utilizes various debugging strategies as well as error checks to ensure that the memory is properly allocated and properly freed.


Special Functions:

void* mymalloc(int size, char* filename, int line): 
This function takes in a integer size value, a filename, and a integer line value. This function takes in a given size of data and allocates space in memory for the given chunk. The malloc checks to see if there is a chunk of space of size in the array to allocate, if there is, it allocates the first space, if not it responds accordingly.

void* myfree(void* addr, char* filename, int line): 
This function takes in an address pointer, a filename, and a line number. Given an address to a space in the memory array, the free function attempts to free the space while also checking for contiguous free space and out of bounds errors. The free function checks to see if the size value stored in the metadata is negative or not, if negative then it is known that the value is already free, if not it will free the space. The free function iterates to each, metadata section finding the specific space needed to free.


Runtime Statistics: 
Workload A ():
malloc() 1 byte and immediately free it - do this 120 times
Time: 0.000003 seconds


Workload B ():
malloc() 1 byte, store the pointer in an array - do this 120 times. Once malloc()ed 120 byte chunks, then free() the 120 1 byte pointers one by one
Time: 0.000055 seconds


Workload C ():
240 times, randomly choose between a 1 byte malloc() or free()ing one of the malloc()ed pointers
Time: 0.000017 seconds


Workload D ():
Assign every other space with a random size of memory and then free the space after filling 120 bytes worth
Time: 0.000015 seconds


Workload E ():
Allocated 3 pointer into the memory, freeing the middle last pointer then the first pointer, then the middle pointer to test the free management.
Time: 0.000000 seconds
