//
// Created by roi on 6/1/23.
//
#include "boundedQueue.h"
#include "article.h"

int initializeQueue(boundedQueue* queue, int size) {
    queue->items = (article**) malloc(sizeof(article) * size);
    queue->size = size;
    if(queue->items == NULL){
        printf("queue allocation failed\n");
        return 1;
    }
    queue->front = -1;
    queue->rear = -1;
    // Initialize the semaphore
    if (sem_init(&queue->empty, 0, queue->size) == -1) {
        perror("Failed to initialize semaphore");
        return 1; // Return an error code
    }
    if (sem_init(&queue->full, 0, 0) == -1) {
        perror("Failed to initialize semaphore");
        return 1; // Return an error code
    }
    // Initialize the mutex
    if (sem_init(&queue->mutex, 0, 1) != 0) {
        printf("Mutex initialization failed\n");
        return 1;  // return an error code indicating failure
    }
    return 0;
}

void boundedEnqueue(boundedQueue* queue,article* newArticle) {
    // Wait for an empty slot in the queue
    sem_wait(&queue->empty);

    // Lock the mutex to ensure exclusive access to the queue
    sem_wait(&queue->mutex);

    queue->rear++;
    queue->items[queue->rear] = newArticle;
    printf("producer enqueue: %d %s %d\n", newArticle->producerNum, newArticle->category, newArticle->counter);

    // Release the mutex to allow other threads to access the queue
    sem_post(&queue->mutex);

    // Signal that a slot has been filled in the queue
    sem_post(&queue->full);
}

article* BoundedDequeue(boundedQueue* queue) {
    article* newArticle;

    // Wait for a filled slot in the queue
    sem_wait(&queue->full);

    // Lock the mutex to ensure exclusive access to the queue
    sem_wait(&queue->mutex);

    // critical section start
    if (queue->front == queue->rear && queue->front != -1) {
        newArticle = queue->items[queue->front];
        queue->rear = -1;
        queue->front = -1;
    } else { // front = -1 rear = 0
        if(queue->front == -1){
            queue->front++;
            newArticle = queue->items[queue->front];
        } else {
            newArticle = queue->items[queue->front];
        }
        queue->front++;
    }
    printf("dispatcher dequeue: %d %s %d\n", newArticle->producerNum, newArticle->category, newArticle->counter);
    // critical section end

    // Release the mutex to allow other threads to access the queue
    sem_post(&queue->mutex);

    // Signal that a slot has been emptied in the queue
    sem_post(&queue->empty);

    return newArticle;
}

void destroyQueue(boundedQueue* queue){
    free(queue->items);
    free(queue);
}
