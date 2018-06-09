#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

int value;
pthread_mutex_t mutex;
pthread_cond_t cond_add;
pthread_cond_t cond_remove;

bool check_if_zero(){
	if (value == 0) 
		return true;
	return false;
}

bool check_if_one(){
	if (value == 1) 
		return true;
	return false;
}

void *add(void *args) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (check_if_one()) {
            pthread_cond_wait(&cond_add, &mutex);
        }
        if (check_if_zero())
			value = 1;
		if (check_if_one())
			pthread_cond_broadcast(&cond_remove);
        pthread_mutex_unlock(&mutex);
    }
	return NULL;
}

void *removee(void *args) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (check_if_zero()) {
            pthread_cond_wait(&cond_remove, &mutex);
        }
        if (check_if_one()){
			value = 0;
			puts("set value to 0");
		}
		if (check_if_zero())
			pthread_cond_broadcast(&cond_add);
        pthread_mutex_unlock(&mutex);
    }
	return NULL;
}


int main() {
	value = 0;
	mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    cond_add = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
    cond_remove = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
	pthread_t remove_;
	pthread_t add_;

	pthread_create(&remove_, NULL, removee, NULL);
	pthread_create(&add_, NULL, add, NULL);

	pthread_join(remove_, NULL);
	pthread_join(add_, NULL);
	
	return 0;
}
