#ifndef STORY_H
#define STORY_H

#include "convention.h"
#include "chapter.h"
#include "queue.h"

typedef struct Story* addressStory;
typedef struct Story{
    int id;
    char title[MAX_TITLE];
    char description[MAX_DESCRIPTION];

    addressStory nextStory;
    Queue chapters;
}Story;

addressStory createStory(char *title, char *description);

void addStory(addressStory *s, addressStory storyNew);

addressStory findStory(addressStory listStory, const char *titleStory);

void deleteAllChapters(Queue* q);

void deleteStoryByTitle(addressStory *headStory, const char *titleStory);

void printAllStory(addressStory s);

char* pilihStoryDariFolder(const char* basePath);

void hapusStory(addressStory *listStory);

void editStory(addressStory listStory);

void hapusFolder(const char* path);

void pilihDanBukaStory(addressStory *listStory);

void tambahChapter(addressStory s);

void pilihChapterDariStory(addressStory s);

void editChapterDariStory(addressStory s);
#endif