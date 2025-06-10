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

// delete chapter

void printChapter(addressChapter ch);

void saveChapterToFile(addressStory s, addressChapter ch);

addressChapter pilihChapter(Queue *q);

void saveChapterWithScenes(addressStory s, addressChapter ch, int chapterIndex);

addressChapter loadChapterFromFile(const char *filepath);

void loadChaptersFromFolder(addressStory s);

void printAllChapter(const Queue *q);

int getChapterCount(Queue *q);

void hapusChapter(addressStory s, int index);

void resaveAllChapters(addressStory s);

#endif