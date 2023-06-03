//
// Created by roi on 6/1/23.
//

#ifndef EX3_ARTICLE_H
#define EX3_ARTICLE_H
#include "stdlib.h"

typedef struct {
    int producerNum;
    char* category;
    int counter;
} article;

article* generateArticle(int producerNum, int* categoryCounters);

#endif //EX3_ARTICLE_H
