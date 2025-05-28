#ifndef QUEUE_H
#define QUEUE_H

#include "chapter.h"

typedef struct {
    addressChapter head;
    addressChapter tail;
} Queue;


bool isQueueEmpty(Queue q);
void createQueue(Queue *q);
void enqueue(Queue *q, addressChapter newCh);
addressChapter dequeue(Queue *q);
void printQueue(const Queue *q);
#endif 