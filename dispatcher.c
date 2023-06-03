//
// Created by roi on 6/2/23.
//

#include "dispatcher.h"

void dispatcherInit(dispatcher* dp, UnboundedQueue** uq, boundedQueue** bq, int prodNum){
    dp->producerNum = prodNum;
    dp->pBoundedQueue = bq;
    dp->pUnboundedQueue = uq;
}

void* dispatcherThread(void* args){
    /*
     * iterate bounded queues with round-robin, BoundedDequeue elements from each
     * and boundedEnqueue each article to the unbounded queues with the same categories
     */
    dispatcher* dp = (dispatcher*) args;
    int* queueStatus = (int*) calloc(dp->producerNum, sizeof(int));
    int queueDoneCount = 0;
    if(queueStatus == NULL){
        printf("failed to allocate queueStatus\n");
        free(queueStatus);
    }
    // run circular on producer queues
    while(1){
        for(int i = 0; i < dp->producerNum; i++){
            // skip queue if it is empty
            if(dp->pBoundedQueue[i]->rear == -1){
                continue;
            } else{
                int front;
                if(dp->pBoundedQueue[i]->front == -1){
                    front = 0;
                } else{
                    front = dp->pBoundedQueue[i]->front;
                }
                if(strcmp(dp->pBoundedQueue[i]->items[front]->category, "DONE") == 0){
                    if(queueStatus[i]){
                        continue;
                    } else {
                        queueStatus[i] = 1;
                        queueDoneCount++;
                        continue;
                    }
                }
                article* extractedArticle = BoundedDequeue(dp->pBoundedQueue[i]);
                if(strcmp(extractedArticle->category, "NEWS") == 0){
                    unboundedEnqueue(dp->pUnboundedQueue[0], extractedArticle);
                } else if(strcmp(extractedArticle->category, "WEATHER") == 0){
                    unboundedEnqueue(dp->pUnboundedQueue[1], extractedArticle);
                } else {
                    unboundedEnqueue(dp->pUnboundedQueue[2], extractedArticle);
                }
            }
        }
        if(queueDoneCount == dp->producerNum){
            article* sentinal = (article*) malloc(sizeof(article));
            if(sentinal == NULL){
                printf("failed to allocate sentinal for unbounded queues\n");
                free(sentinal);
                return NULL;
            }
            sentinal->category = "DONE";
            sentinal->producerNum = 0;
            sentinal->counter = 0;
            unboundedEnqueue(dp->pUnboundedQueue[0], sentinal);
            unboundedEnqueue(dp->pUnboundedQueue[1], sentinal);
            unboundedEnqueue(dp->pUnboundedQueue[2], sentinal);
            return NULL;
        }
    }
}
