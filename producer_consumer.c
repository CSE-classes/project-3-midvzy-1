#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define BUFFER_SIZE 5

char buffer[BUFFER_SIZE];
int in = 0;   // producer inserts here
int out = 0;  // consumer reads here
int count = 0; // number of items in buffer

pthread_mutex_t mutex;
pthread_cond_t not_full;
pthread_cond_t not_empty;

char message[1024];
int msg_len = 0;

void *producer(void *arg)
{
    int i;
    for(i = 0; i < msg_len; i++){
        pthread_mutex_lock(&mutex);

        while(count == BUFFER_SIZE)
            pthread_cond_wait(&not_full, &mutex);

        buffer[in] = message[i];
        in = (in + 1) % BUFFER_SIZE;
        count++;

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}

void *consumer(void *arg)
{
    int i;
    for(i = 0; i < msg_len; i++){
        pthread_mutex_lock(&mutex);

        while(count == 0)
            pthread_cond_wait(&not_empty, &mutex);

        char ch = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);

        printf("%c", ch);
    }
    printf("\n");
    pthread_exit(0);
}

int main()
{
    FILE *fp = fopen("message.txt", "r");
    if(fp == NULL){
        printf("ERROR: can't open message.txt!\n");
        return -1;
    }
    fgets(message, 1024, fp);
    fclose(fp);
    msg_len = strlen(message);

    pthread_t prod_thread, cons_thread;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);

    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    return 0;
}
