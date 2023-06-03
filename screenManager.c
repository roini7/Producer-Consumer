//
// Created by roi on 6/3/23.
//

#include "screenManager.h"

void* screenFunc(void* args){
        boundedQueue* mainQueue = (boundedQueue*) args;
        int counter = 0;
        while(1){
            if(counter == 3){
                break;
            }
            article* article = BoundedDequeue(mainQueue);
            if(strcmp(article->category, "DONE") == 0){
                counter++;
            } else{
                printf("%d %s %d\n", article->producerNum, article->category, article->counter);
            }
        }
        return NULL;
}
