#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "chapter.h"

typedef struct {
    addressChapter head;
    addressChapter tail;
} Queue;


bool isQueueEmpty(Queue q);
void initQueue(Queue *q);
void enqueue(Queue *q, addressChapter newCh);
addressChapter dequeue(Queue *q);
void printQueue(const Queue *q);
#endif 