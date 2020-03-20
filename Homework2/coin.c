//coin.c
//Problem 2.1)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

static int coins[20], GlobalLock1 = 1, GlobalLock2 = 1, CoinLock[20], P = 100, N = 10000, coins[20];

void* GlobalLockForPerson(void* data){
	if(GlobalLock1 == 0){sleep(1);}
	GlobalLock1 = 0;
	for (int i = 0; i < 20; ++i)
	{
		for (int j = 0; j < N; ++j)
		{	
			coins[i] = rand()%2;			
		}	
	}
	GlobalLock1 = 1;
	return NULL;
}

void* GlobalLockForIteration(void* data){
	for (int j = 0; j < N; ++j)
	{
		if(GlobalLock2 == 0){sleep(1);}
		GlobalLock2 = 0;
		for (int i = 0; i < 20; ++i)
		{
				coins[i] = rand()%2;				
		}
		GlobalLock2 = 1;
	}
	return NULL;
}

void CoinLockHelper(int i){
	if(CoinLock[i] == 0){sleep(1);}
	CoinLock[i] = 0;
	coins[i] = rand()%2;				
	CoinLock[i] = 1;
	return;
}
void* CoinLockLocking(){
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < 20; ++j)
		{
			CoinLockHelper(j);
		}
	}
	return NULL;
}
void run_threads(void*(*proc)(void*)){
	int r;
	pthread_t tids[N];
	for (int i = 0; i < P; ++i)
	{

		r = pthread_create(&tids[i], NULL, proc, NULL);
		if(r){
			fprintf(stderr, "Failed to create thread %s\n", strerror(r));
		}
	}
	for (int i = 0; i < P; ++i)
	{
		if(tids[i]){
			(void) pthread_join(tids[i], NULL);
		}
	}

}

static double timeit(void*(*proc)(void*)){
	clock_t t1, t2;
	t1 = clock();
	run_threads(proc);
	t2 =  clock();
	return((double)t2-(double)t1) / CLOCKS_PER_SEC * 1000; 
}

void printcoins(){
	printf("coins: ");
	for (int i = 0; i < 20; ++i)
	{
		if(!coins[i]){
			printf("0");
		}
		else{
			printf("X");
		}
	}
	return;
}

int main(int argc, char* argv[]){
	srand(time(0));
	memset(CoinLock, 1, 20*sizeof(CoinLock[20]));

	for (int i = 0; i < 20; ++i)
	{
		coins[i] = rand()%2;
	}
	char option;
	
	while((option = getopt(argc, argv,"p:n:")) != -1){
		switch(option){
			case 'p':
				P = atoi(optarg);
				continue;
			
			case 'n':
				N = atoi(optarg);
				continue;

			case '?':
				//perror("Argument requires a value \n");
				exit(EXIT_FAILURE);
		}
	}

	printcoins();
	printf("    (start -global lock)\n");
	double t1 = timeit(GlobalLockForPerson);
	printcoins();
	printf("    (end -global lock)\n");
	printf("%d threads x %d flips: %f",P,N,t1);
	printf("\n\n");

	printcoins();
	printf("    (start -iteration lock)\n");
	t1 = timeit(GlobalLockForIteration);
	printcoins();
	printf("    (end -iteration lock)\n");
	printf("%d threads x %d flips: %f",P,N,t1);
	printf("\n\n");

	printcoins();
	printf("    (start -coin lock)\n");
	t1 = timeit(CoinLockLocking);
	printcoins();
	printf("    (end -coin lock)\n");
	printf("%d threads x %d flips: %f",P,N,t1);
	printf("\n\n");

}




