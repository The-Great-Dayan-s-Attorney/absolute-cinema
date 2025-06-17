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

void trim(char* str);
FileManager* createFileManager();
void fm_save_story(addressStory s);
void fm_save_chapter(addressStory s, addressChapter ch);
void fm_save_chapter_with_scenes(addressStory s, addressChapter ch, int chapterIndex);
addressStory fm_load_story(char* filename);
addressChapter fm_load_chapter_from_file(const char *filepath);
void fm_load_chapters_from_folder(addressStory s);
void fm_load_all_stories(addressStory* listStory);
addressChapter fm_load_chapter(addressStory s, int chapterIndex);
void fm_resave_all_chapters(addressStory s);
StoryEntry* listStories(int* count);
int validateFile(char* filename);
void deleteStory(char* filename);
void updateStoryFile(addressStory story, char* filename);

#endif