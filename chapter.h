#ifndef CHAPTER_H
#define CHAPTER_H

#include <stdio.h>
#include <stdlib.h>

#include "convention.h"

typedef struct Chapter* chapter;
typedef struct Chapter{
    int id;
    char title [MAX_TITLE];
    char description [MAX_DESCRIPTION];

    chapter nextChapter;
}Chapter;

#endif