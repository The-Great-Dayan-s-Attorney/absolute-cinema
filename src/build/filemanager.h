#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "convention.h"

typedef struct Story* addressStory;
typedef struct Chapter* addressChapter;
typedef struct Scene* addressScene;

typedef struct {
    char filename[MAX_NAME];
    char title[MAX_TITLE];
} StoryEntry;

typedef struct FileManager {
    int dummy;
} FileManager;

FileManager* createFileManager();
void saveStory(addressStory story, char* filename);
addressStory loadStory(char* filename);
StoryEntry* listStories(int* count);
int validateFile(char* filename);
void deleteStory(char* filename);
void updateStoryFile(addressStory story, char* filename);

#endif