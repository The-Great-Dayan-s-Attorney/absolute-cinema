#ifndef CHAPTER_H
#define CHAPTER_H

#include <stdio.h>
#include <stdlib.h>

#include "convention.h"
#include "story.h"
#include "scene.h"

typedef struct Chapter* addressChapter;
typedef struct Chapter{
    char title [MAX_TITLE];
    char description [MAX_DESCRIPTION];

    addressChapter nextChapter;
    addressScene firstScene;
}Chapter;

addressChapter createChapter(const char *title, const char *desc);

void deleteAllChapters(addressChapter ch);

void printChapter(addressChapter ch);

#endif