#ifndef CHAPTER_H
#define CHAPTER_H

#include <stdio.h>
#include <stdlib.h>

#include "convention.h"
#include "story.h"
#include "scene.h"
#include "queue.h"

typedef struct Story* addressStory;

typedef struct Chapter* addressChapter;
typedef struct Chapter{
    char title [MAX_TITLE];
    char description [MAX_DESCRIPTION];

    addressChapter nextChapter;
    addressScene firstScene;
}Chapter;

addressChapter createChapter(const char *title, const char *desc);

void deleteChapter(addressStory s, int index);

void printChapter(addressChapter ch);

addressChapter pilihChapter(Queue *q);

void printAllChapter(const Queue *q);

int getChapterCount(Queue *q);

#endif