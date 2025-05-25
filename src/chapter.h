#ifndef CHAPTER_H
#define CHAPTER_H

#include <stdio.h>
#include <stdlib.h>

#include "convention.h"
#include "story.h"

typedef struct Chapter* addressChapter;
typedef struct Chapter{
    int id;
    char title [MAX_TITLE];
    char description [MAX_DESCRIPTION];

    addressChapter nextChapter;
}Chapter;

void deleteAllChapters(addressChapter ch);

#endif