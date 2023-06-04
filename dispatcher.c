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
        return NULL;
    }
    // run circular on producer queues
    while(1){
        for(int i = 0; i < dp->producerNum; i++){
            article* extractedArticle = BoundedDequeue(dp->pBoundedQueue[i]);
            if(strcmp(extractedArticle->category, "DONE") == 0) {
                free(extractedArticle);
                if(queueStatus[i]){
                    continue;
                } else {
                    queueStatus[i] = 1;
                    queueDoneCount++;
                    continue;
                }
            }else if(strcmp(extractedArticle->category, "NEWS") == 0){
                unboundedEnqueue(dp->pUnboundedQueue[0], extractedArticle);
            } else if(strcmp(extractedArticle->category, "WEATHER") == 0){
                unboundedEnqueue(dp->pUnboundedQueue[1], extractedArticle);
            } else {
                unboundedEnqueue(dp->pUnboundedQueue[2], extractedArticle);
            }
        }
        if(queueDoneCount == dp->producerNum){
            // enqueue sentinal values to unbounded buffers
            for(int i = 0; i < 3 ;i++){
                article* sentinal = (article*) malloc(sizeof(article));
                sentinal->category = "DONE";
                sentinal->producerNum = i;
                sentinal->counter = 0;
                unboundedEnqueue(dp->pUnboundedQueue[i], sentinal);
            }
            free(queueStatus);
            return NULL;
        }
    }
}
