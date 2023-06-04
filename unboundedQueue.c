//
// Created by roi on 6/1/23.
//

#include "unboundedQueue.h"
void initBuffer(UnboundedQueue* buffer) {
    buffer->front = NULL;
    buffer->rear = NULL;
    sem_init(&(buffer->empty), 0, 0);
    sem_init(&(buffer->mutex), 0, 1);
}

void unboundedEnqueue(UnboundedQueue * buffer, article* newArticle) {
    node* newNode = (node*)malloc(sizeof(node));
    newNode->article = newArticle;
    newNode->next = NULL;

    sem_wait(&(buffer->mutex));

//    printf("dispatcher enqueue: %d %s %d\n", newArticle->producerNum, newArticle->category, newArticle->counter);
    if (buffer->front == NULL) {
        buffer->front = newNode;
        buffer->rear = newNode;
    } else {
        buffer->rear->next = newNode;
        buffer->rear = newNode;
    }

    sem_post(&(buffer->empty));
    sem_post(&(buffer->mutex));
}

article* unboundedDequeue(UnboundedQueue* buffer) {
    sem_wait(&(buffer->empty));
    sem_wait(&(buffer->mutex));
    node* removedNode = buffer->front;
    article* removedArticle = removedNode->article;
//    printf("co-editor dequeue: %d %s %d\n", removedArticle->producerNum, removedArticle->category, removedArticle->counter);

    buffer->front = buffer->front->next;

    free(removedNode);
    sem_post(&(buffer->mutex));

    return removedArticle;
}


void destroyBuffer(UnboundedQueue* buffer) {
    node* current = buffer->front;
    node* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    sem_destroy(&(buffer->empty));
    sem_destroy(&(buffer->mutex));
    free(buffer);
}

void printUnboundedQueue(UnboundedQueue* uq){
    node* current = uq->front;
    while(current->next != NULL){
        printf("%d %s %d\n", current->article->producerNum, current->article->category, current->article->counter);
        current = current->next;
    }
    printf("%d %s %d\n", current->article->producerNum, current->article->category, current->article->counter);
}
