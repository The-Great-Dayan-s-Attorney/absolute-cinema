#ifndef STORY_H
#define STORY_H

#include "convention.h"
#include "chapter.h"

typedef struct Story* addressStory;
typedef struct Story{
    int id;
    char title[MAX_TITLE];
    char description[MAX_DESCRIPTION];

    addressStory nextStory;
    addressChapter firstChapter;
}Story;

addressStory createStory(char *title, char *description);

void addStory(addressStory *s, addressStory storyNew);

addressStory findStory(addressStory listStory, const char *titleStory);

void deleteStoryByTitle(addressStory *headStory, const char *titleStory);

void printAllStory(addressStory s);

void writeStoryToFolder(addressStory s);

#endif