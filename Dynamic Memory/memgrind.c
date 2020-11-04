#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
#include<string.h>
#include<sys/time.h>
#include<ctype.h>

double workloadA(){
	int i ;
	double runtime = 0;
	struct timeval time1, time2;
	void* arr[4096];
	gettimeofday(&time1, NULL);
	for(i = 0; i < 120; i++){
		arr[i] = malloc(1);
		free(arr[i]);
	}
	gettimeofday(&time2, NULL);
	runtime = (double) (time2.tv_usec - time1.tv_usec)/1000000 + (double) (time2.tv_sec - time1.tv_sec);
	return runtime;
}
double workloadB(){
	int i;
	int j;
	double runtime=0;
	struct timeval time1, time2;
	void* arr[4096];
	gettimeofday(&time1, NULL);
	for(i = 0; i < 120; i++){
		arr[i] = malloc(1);
		
	}
	for(j= 0;j<120;j++){
		free(arr[j]);
	}
	gettimeofday(&time2, NULL);
	runtime = (double) (time2.tv_usec - time1.tv_usec)/1000000 + (double) (time2.tv_sec - time1.tv_sec);
	return runtime;


}
double workloadC(){
	int i;
	
	struct timeval time1, time2;

	int countMallocs=0;
	int countFrees=0;
	double runtime=0;
	void* chPtr[4096];
	gettimeofday(&time1, NULL);
	for(i=0;i<240;i++){
	while (countMallocs!= 120)
	{
		int n = rand() % 2;
		if(n == 0)
		{
			chPtr[countMallocs] = malloc(1);
			countMallocs++;
		}
		else
		{
			if(countMallocs>countFrees)
			{
				free(chPtr[countFrees]);
				countFrees++;
			}
		}
	}
	while(countFrees!=countMallocs)
	{
		free(chPtr[countFrees]);
		countFrees++;
	}
	}
	
 	gettimeofday(&time2, NULL);
	runtime = (double) (time2.tv_usec - time1.tv_usec)/1000000 + (double) (time2.tv_sec - time1.tv_sec);
	return runtime;
}
//mallocs every other space with a size from 1-3 and then free's every other
double workloadD(){
	int i=0;
	int j=0;
	double runtime=0;
	struct timeval time1, time2;
	gettimeofday(&time1, NULL);
	void* chPtr[4096];
	
		int n = rand() % 3+1;
		while(i<120){
			chPtr[i] = malloc(n);
			i=i+2;
			
		}
		while(j<120){
				free(chPtr[j]);
				j=j+2;
		
		}
 	gettimeofday(&time2, NULL);
	runtime = (double) (time2.tv_usec - time1.tv_usec)/1000000 + (double) (time2.tv_sec - time1.tv_sec);
	return runtime;
}
double workloadE(){
	double runtime=0;

	struct timeval time1, time2;
	gettimeofday(&time1, NULL);
	int* ptr = (int*)malloc(200);
	int* ptr2 = (int*)malloc(400);
	int* ptr3 = (int*) malloc(1000);
	free(ptr3);
	free(ptr);	
	free(ptr2);	
 	gettimeofday(&time2, NULL);
	runtime = (double) (time2.tv_usec - time1.tv_usec)/1000000 + (double) (time2.tv_sec - time1.tv_sec);
	return runtime;

}


int main(){
	
	int i;
	double runTimeA=0;
	double runTimeB=0;
	double runTimeC=0;
	double runTimeD=0;
	double runTimeE=0;
	double runTimeF=0;
	for(i=0; i < 50; i++){
		//Workload A
		runTimeA += workloadA();
		//Workload B
		runTimeB+= workloadB();
		//Workload C
		runTimeC+=workloadC();
		//Workload D
		runTimeD+=workloadD();
		//Workload E
		runTimeE+=workloadE();

		runTimeF+=workloadF();



	}
	printf("Mean Runtime for Workload A: %f seconds\n", runTimeA/50);
	printf("Mean Runtime for Workload B: %f seconds\n", runTimeB/50);
	printf("Mean Runtime for Workload C: %f seconds\n", runTimeC/50);
	printf("Mean Runtime for Workload D: %f seconds\n", runTimeD/50);
	printf("Mean Runtime for Workload E: %f seconds\n", runTimeE/50);
	printf("Mean Runtime for Workload F: %f seconds\n", runTimeF/50);


}