/**
 * 
 * Implementation of a detective meeting client problem
 * @assignment  Homework Sheet 3
 * @problem     b)
 * 
 * @author      Dipak kandel
 * 
 * program usage: ./bar -c[no of clients] -d[no of detectives]
 *
 *remarks: remove "break" at line 166 to have a open loop as suggested in the question
 * 
 * 
*/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <pthread.h>

enum type_t{client, detective};

static const char *programe = "bar";


typedef struct Bar{
	unsigned int detective_count, client_count, total_visited_count;
	int next_detective, next_client;
	pthread_mutex_t mutex;                                           
	pthread_mutex_t  barrier;
	pthread_cond_t detective;
	pthread_cond_t client;

}bar_t;

static bar_t instance_bar = {
	.detective_count = 0,
	.total_visited_count = 0,
	.client_count = 0,
	.next_detective = 0,
	.next_client = 0,
	.barrier = PTHREAD_MUTEX_INITIALIZER,
	.mutex = PTHREAD_MUTEX_INITIALIZER,
	.detective = PTHREAD_COND_INITIALIZER,
	.client = PTHREAD_COND_INITIALIZER
};

typedef struct visitor{
	int info;
	bar_t *bar;
	enum type_t type; 
}visitor_t;

void print_status(int c, int d){
	printf("%s:     %d c     %d d	    ",programe,c,d);
	return;
}

void client_visit_bar(void *data){
	visitor_t *person = (visitor_t*) data;
	int check = 1;
	(void) pthread_mutex_lock(&person->bar->barrier);
	(void) pthread_mutex_lock(&person->bar->mutex);
	person->bar->total_visited_count++;
	person->info = person->bar->total_visited_count-1;
	print_status(person->bar->client_count, person->bar->detective_count);
	printf("c%d entering\n",person->info);
	person->bar->client_count++;
	(void) pthread_cond_signal(&person->bar->client);
	if(!person->bar->detective_count){
		check = 1;
		print_status(person->bar->client_count, person->bar->detective_count);
		printf("c%d waiting..\n",person->info);
		(void) pthread_mutex_unlock(&person->bar->barrier);
		(void) pthread_cond_wait(&person->bar->detective, &person->bar->mutex);
		person->bar->next_client = person->info;	
		print_status(person->bar->client_count, person->bar->detective_count);
		printf("...c%d waking up\n",person->info);		
	}else{
		check = 0;
		print_status(person->bar->client_count, person->bar->detective_count);
		printf("c%d picking the first detective \n",person->info);

	}
	person->bar->client_count--;
	
	
	print_status(person->bar->client_count, person->bar->detective_count);
	printf("c%d leaving\n",person->info);
	if((person->bar->client_count == 0) && check){
		(void) pthread_mutex_unlock(&person->bar->barrier);		
	}

	(void) pthread_mutex_unlock(&person->bar->mutex);
	return;

}

void detective_visit_bar(void *data){
	visitor_t *person = (visitor_t*) data;
	int check = 0;
	(void) pthread_mutex_lock(&person->bar->barrier);
	(void) pthread_mutex_lock(&person->bar->mutex);

	person->bar->total_visited_count++;
	person->info = person->bar->total_visited_count-1;

	print_status(person->bar->client_count, person->bar->detective_count);
	printf("d%d entering\n", person->info);
	person->bar->detective_count++;

	if(!person->bar->client_count){
		check = 1;
		print_status(person->bar->client_count, person->bar->detective_count);
		printf("d%d waiting..\n",person->info);
		(void) pthread_mutex_unlock(&person->bar->barrier);
		(void) pthread_cond_wait(&person->bar->client, &person->bar->mutex);
		person->bar->next_detective = person->info;
		print_status(person->bar->client_count, person->bar->detective_count);
		printf("...d%d waking up\n",person->info);
	}

	for (int i = 0; i < person->bar->client_count; ++i){
		print_status(person->bar->client_count, person->bar->detective_count);
		printf("d%d picking client c%d\n",person->info,person->bar->next_client);	
		person->bar->next_client++;
	}
	(void)pthread_cond_broadcast(&person->bar->detective);
	person->bar->detective_count--;
	print_status(person->bar->client_count, person->bar->detective_count);
	printf("d%d leaving\n", person->info);	

	if(check){
		(void) pthread_mutex_unlock(&person->bar->barrier);		
	}		
	(void) pthread_mutex_unlock(&person->bar->mutex);
	return;

}




static void* enjoy_life(void *data){

	visitor_t *person = (visitor_t*) data;
	while(1){
		switch(person->type){
			case client:
				(void) client_visit_bar(person); break;
			case detective:
				(void) detective_visit_bar(person); break;
		}
		break;
	usleep(random()%100000);
	}
	return NULL;
}


int main(int argc, char *argv[]){
	srand(time(0));
	int a, c = 1, d = 1;
	while ((a = getopt(argc, argv, "c:d:h")) >= 0) {
		switch (a){
			case 'c':
			if ((c = atoi(optarg)) <= 0) {
				fprintf(stderr, "number of clients must be > 0\n");
				exit(EXIT_FAILURE);
			}
			break;

			case 'd':
			if ((d = atoi(optarg)) <= 0) {
			fprintf(stderr, "number of detective must be > 0\n");
			exit(EXIT_FAILURE);
			}
			break;

			case 'h':
			printf("Usage: %s [-c clients] [-d detective] [-h]\n", programe);
			exit(EXIT_SUCCESS);
		}
	}

	int err, n = c + d;
	pthread_t thread[n];
	visitor_t person[n];

	for (int i = 0; i < n; i++) {
			if(i < c){
				person[i].bar = &instance_bar;
				person[i].type = client;
				err = pthread_create(&thread[i], NULL, enjoy_life, &person[i]);
			}else{
				person[i].bar = &instance_bar;
				person[i].type = detective;
				err = pthread_create(&thread[i], NULL, enjoy_life, &person[i]);				
			}

		if (err) {
			
			fprintf(stderr, "%s: %s(): unable to create thread %d: %s\n", programe, __func__, i, strerror(err));
			return EXIT_FAILURE;
		}
	}

	for (int i = 0; i < n; i++) {
		if (thread[i])
			err = pthread_join(thread[i], NULL);
		if (err)
			fprintf(stderr, "%s: %s(): unable to join thread %d: %s\n", programe, __func__, i, strerror(err));
	}
	return EXIT_SUCCESS;
}
