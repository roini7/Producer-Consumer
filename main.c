#include <stdlib.h>
#include <stdio.h>
#include "pthread.h"
#include "semaphore.h"
#include "string.h"
#include "producer.h"
#include "boundedQueue.h"
#include "unboundedQueue.h"
#include "dispatcher.h"

#define CATEGORIES_NUM 3
#define BUFFER_SIZE 10

int allocationCheck(void* allocation, char* type){
    if(allocation == NULL){
        printf("failed to allocate %s\n", type);
        free(allocation);
        return 1;
    } else{
        return 0;
    }
}

//return producer array
producer** configHandler(char* path, int* ptrProdCount) {
    producer** producers = (producer**) malloc(sizeof(producer) * BUFFER_SIZE);
    if(allocationCheck((void*) producers, "producers")){
        return NULL;
    }
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        // Failed to open the file
        // Handle the error
        printf("failed to open file\n");
        return NULL;
    }
    int args[3];
    char line[100];  // Assume each line in the file is at most 100 characters long
    int counter = 0;
    int bufferSize = BUFFER_SIZE;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == '\n') {
            producer * producer1 = (producer*) malloc(sizeof(producer));
            producer1->producerNum = args[0];
            producer1->productsNum = args[1];
            producer1->queueSize = args[2];
            // add the producer to the producers array
            if(*ptrProdCount > bufferSize){
                bufferSize *= 2;
                producer** tempProducers = realloc(producers, sizeof(producer) * bufferSize);
                if(allocationCheck((void*) tempProducers, "producers")){
                    return NULL;
                }
                producers = tempProducers;
            }
            producers[*ptrProdCount] = producer1;
            (*ptrProdCount)++;
            continue;  // Skip empty lines
        }
        // Parse the block of text and extract the three integers
        sscanf(line, "%d", &args[counter]);
        counter = (counter + 1) % 3;
    }
    fclose(file);
    return producers;
}


int main(int argc, char* argv[]) {
    if(argc != 2){
        printf("Invalid number of arguments\n");
        return 1;
    } else {
        int producerCount = 0;
        int* ptrProdSize = &producerCount;
        producer** producers = configHandler(argv[1], ptrProdSize);
        // bounded queue array to use later
        boundedQueue** boundedQueues = (boundedQueue**) malloc(sizeof(boundedQueue) * (*ptrProdSize));
        if(allocationCheck((void*) boundedQueues, "producers")){
            return 1;
        }
        // thread group id for producers
        pthread_t* t_gid = (pthread_t*) malloc(sizeof(pthread_t) * (*ptrProdSize));
        for(int i = 0; i < *ptrProdSize; i++){
            // initialize Queue for producer
            boundedQueue* newQueue = (boundedQueue*) malloc(sizeof(boundedQueue));
            if(allocationCheck((void*) newQueue, "bounded Queue")){
                return 1;
            }
            initializeQueue(newQueue,producers[i]->queueSize);
            producers[i]->producerQueue = newQueue;
            // assign to bounded queue
            boundedQueues[i] = newQueue;
            pthread_t thread_id;
            int result = pthread_create(&thread_id, NULL,
                                        producerThread, (void*) producers[i]);
            t_gid[i] = thread_id;
            if (result != 0) {
                printf("Failed to create thread: %d\n", result);
            }
        }
        // Initialize unbounded queues
        UnboundedQueue** unboundedQueues = (UnboundedQueue**) malloc(sizeof(unboundedQueues) * CATEGORIES_NUM);
        if(allocationCheck((void*) unboundedQueues, "producers")){
            return 1;
        }
        for(int i = 0; i < CATEGORIES_NUM; i++){
            UnboundedQueue* newQueue = (UnboundedQueue*) malloc(sizeof(UnboundedQueue));
            if(allocationCheck((void*) unboundedQueues, "producers")){
                return 1;
            }
            initBuffer(newQueue);
            unboundedQueues[i] = newQueue;
        }
        // initialize dispatcher and create a thread
        dispatcher* dp = (dispatcher*) malloc(sizeof(dispatcher));
        dispatcherInit(dp, unboundedQueues, boundedQueues, (*ptrProdSize));
        pthread_t dispatcher_thread;
        int result = pthread_create(&dispatcher_thread, NULL,
                                    dispatcherThread, (void*) dp);
        if (result != 0) {
            printf("Failed to create thread: %d\n", result);
        }

        // Join producer threads, move after dispatcher
        for(int i = 0; i < *ptrProdSize; i++){
            pthread_join(t_gid[i], NULL);
        }
        pthread_join(dispatcher_thread, NULL);
        return 0;
    }
}
