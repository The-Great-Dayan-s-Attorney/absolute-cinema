
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "queue.h"

bool isQueueEmpty(Queue q) {
    return (q.head == NULL);
}


void initQueue(Queue *q) {
    q->head = NULL;
    q->tail = NULL;
}


void enqueue(Queue *q, addressChapter newCh) {
    newCh->nextChapter = NULL;
    if (isQueueEmpty(*q)) {
        q->head = newCh;
        q->tail = newCh;
    } else {
        q->tail->nextChapter = newCh;
        q->tail = newCh;
    }
}


addressChapter dequeue(Queue *q) {
    if (isQueueEmpty(*q)) {
        return NULL;
    }

    addressChapter removed = q->head;
    q->head = q->head->nextChapter;

    if (q->head == NULL) {
        q->tail = NULL;
    }

    removed->nextChapter = NULL;
    return removed;
}

void printQueue(const Queue *q) {
    if (isQueueEmpty(*q)) {
        printf("Queue kosong.\n");
        return;
    }

    addressChapter current = q->head;
    int index = 1;
    while (current != NULL) {
        printf("%d. Title: %s\n", index, current->title);
        printf("   Description: %s\n", current->description);
        current = current->nextChapter;
        index++;
    }
}