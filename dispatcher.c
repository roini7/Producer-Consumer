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
    int queueDoneCount = 0;
    // run circular on producer queues
    while(1){
        for(int i = 0; i < dp->producerNum; i++){
            if(dp->pBoundedQueue[i]->done){
                continue;
            }
            article* extractedArticle = BoundedDequeue(dp->pBoundedQueue[i]);
            if(strcmp(extractedArticle->category, "DONE") == 0) {
                free(extractedArticle);
                if(dp->pBoundedQueue[i]->done){
                    continue;
                } else {
                    dp->pBoundedQueue[i]->done = 1;
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
            return NULL;
        }
    }
}
