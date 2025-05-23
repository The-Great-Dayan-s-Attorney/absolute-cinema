#ifndef STORY_H
#define STORY_H

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

story createStory(char *title, char *description);

void addStory(story *s, story storyNew);

void printAllStory(story s);

void writeStoryToFolder(story s);

#endif