#ifndef STORY_H
#define STORY_H

#include <stdio.h>
#include <stdlib.h>

#include "convention.h"
#include "chapter.h"

typedef struct Story* story;
typedef struct Story{
    int id;
    char title[MAX_TITLE];
    char description[MAX_DESCRIPTION];

    story nextStory;
    chapter firstChapter;
}Story;

#endif