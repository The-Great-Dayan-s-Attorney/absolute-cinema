#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "filemanager.h"
#include "story.h"
#include "chapter.h"
#include "scene.h"

// Helper untuk menghapus newline
static void trim(char* str) {
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
}

FileManager* createFileManager() {
    FileManager* fm = malloc(sizeof(FileManager));
    if (fm == NULL) {
    return NULL;
    }
    fm->dummy = 0;
    return fm;
}

void saveStory(addressStory story, char* filename) {
    if (story == NULL || filename == NULL) return;

    char folderPath[MAX_NAME];
    snprintf(folderPath, MAX_NAME, "data/%s", story->title);

    char detailsPath[MAX_NAME];
    snprintf(detailsPath, MAX_NAME, "%s/details_story.txt", folderPath);
    FILE* file = fopen(detailsPath, "w");
    if (file == NULL) return;
    fprintf(file, "Judul: %s\n", story->title);
    fprintf(file, "Deskripsi: %s\n", story->description);
    fclose(file);

    addressChapter chapter = story->chapters.head;
    int chapterNum = 1;
    while (chapter != NULL) {
        char chapterPath[MAX_NAME];
        snprintf(chapterPath, MAX_NAME, "%s/chapter_%d.txt", folderPath, chapterNum);
        chapterNum++;
        FILE* chapterFile = fopen(chapterPath, "w");
        if (chapterFile == NULL) return;

        fprintf(chapterFile, "Title: %s\n", chapter->title);
        fprintf(chapterFile, "Description: %s\n", chapter->description);

        addressScene scene = chapter->firstScene;
        while (scene != NULL) {
            char choices[100] = "";
            char ids[50] = "";
            for (int i = 0; i < MAX_CHOICES; i++) {
                if (scene->choices[i].title[0] != '\0') {
                    if (i > 0) {
                        strcat(choices, ",");
                        strcat(ids, ",");
                    }
                    strcat(choices, scene->choices[i].title);
                    char idStr[10];
                    snprintf(idStr, 10, "%d", scene->choices[i].id);
                    strcat(ids, idStr);
                }
            }
            fprintf(chapterFile, "Scene: %s|%s|%s|%s\n",
                    scene->title, scene->description, choices, ids);
            scene = scene->nextScene; // Sudah benar berdasarkan definisi Scene
        }
        fclose(chapterFile);
        chapter = chapter->nextChapter;
    }
}

addressStory loadStory(char* filename) {
    if (filename == NULL) return NULL;

    char folderPath[MAX_NAME];
    snprintf(folderPath, MAX_NAME, "data/%s", filename);

    char detailsPath[MAX_NAME];
    snprintf(detailsPath, MAX_NAME, "%s/details_story.txt", folderPath);
    FILE* file = fopen(detailsPath, "r");
    if (file == NULL) return NULL;

    char line[1000];
    char title[MAX_TITLE] = "";
    char description[MAX_DESCRIPTION] = "";
    while (fgets(line, 1000, file)) {
        trim(line);
        if (strncmp(line, "Judul: ", 7) == 0) strncpy(title, line + 7, MAX_TITLE - 1);
        else if (strncmp(line, "Deskripsi: ", 11) == 0) strncpy(description, line + 11, MAX_DESCRIPTION - 1);
    }
    fclose(file);

    if (title[0] == '\0') return NULL;
    addressStory story = createStory(title, description);
    if (story == NULL) return NULL;

    int chapterNum = 1;
    char chapterPath[MAX_NAME];
    snprintf(chapterPath, MAX_NAME, "%s/chapter_%d.txt", folderPath, chapterNum);
    FILE* chapterFile = fopen(chapterPath, "r");
    addressChapter lastChapter = NULL;

    while (chapterFile != NULL) {
        addressChapter chapter = malloc(sizeof(Chapter));
        if (chapter == NULL) {
            fclose(chapterFile);
            return NULL;
        }
        chapter->nextChapter = NULL;
        chapter->firstScene = NULL;
        if (story->chapters.head == NULL) story->chapters.head = chapter;
        else lastChapter->nextChapter = chapter;
        lastChapter = chapter;

        addressScene lastScene = NULL;
        while (fgets(line, 1000, chapterFile)) {
            trim(line);
            if (strncmp(line, "Title: ", 7) == 0) {
                strncpy(chapter->title, line + 7, MAX_TITLE - 1);
            } else if (strncmp(line, "Description: ", 13) == 0) {
                strncpy(chapter->description, line + 13, MAX_DESCRIPTION - 1);
            } else if (strncmp(line, "Scene: ", 7) == 0) {
                char title[MAX_TITLE], desc[MAX_DESCRIPTION], choices[100], ids[50];
                if (sscanf(line, "Scene: %[^|]|%[^|]|%[^|]|%[^\n]", title, desc, choices, ids) < 4) continue;
                addressScene scene = createScene(title, desc);
                if (scene == NULL) continue;

                char* choice = strtok(choices, ",");
                char* id = strtok(ids, ",");
                int i = 0;
                while (choice && id && i < MAX_CHOICES) {
                    strncpy(scene->choices[i].title, choice, MAX_TITLE - 1);
                    scene->choices[i].id = atoi(id);
                    i++;
                    choice = strtok(NULL, ",");
                    id = strtok(NULL, ",");
                }

                if (chapter->firstScene == NULL) chapter->firstScene = scene;
                else lastScene->nextScene = scene;
                lastScene = scene;
            }
        }
        fclose(chapterFile);

        chapterNum++;
        snprintf(chapterPath, MAX_NAME, "%s/chapter_%d.txt", folderPath, chapterNum);
        chapterFile = fopen(chapterPath, "r");
    }

    return story;
}

StoryEntry* listStories(int* count) {
    *count = 0;
    StoryEntry* stories = malloc(MAX_STORY * sizeof(StoryEntry));
    if (stories == NULL) {
        return NULL;
    }
    DIR* dir = opendir("data");
    if (dir == NULL) {
        free(stories);
        return NULL;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char folderPath[MAX_NAME];
            snprintf(folderPath, MAX_NAME, "data/%s", entry->d_name);
            char detailsPath[MAX_NAME];
            snprintf(detailsPath, MAX_NAME, "%s/details_story.txt", folderPath);
            FILE* file = fopen(detailsPath, "r");
            if (file != NULL) {
                char line[1000];
                while (fgets(line, 1000, file)) {
                    trim(line);
                    if (strncmp(line, "Judul: ", 7) == 0) {
                        strncpy(stories[*count].filename, entry->d_name, MAX_NAME - 1);
                        strncpy(stories[*count].title, line + 7, MAX_TITLE - 1);
                        (*count)++;
                        break;
                    }
                }
                fclose(file);
            }
        }
    }
    closedir(dir);
    return stories;
}

int validateFile(char* filename) {
    if (filename == NULL) return 0;

    char folderPath[MAX_NAME];
    snprintf(folderPath, MAX_NAME, "data/%s", filename);
    char detailsPath[MAX_NAME];
    snprintf(detailsPath, MAX_NAME, "%s/details_story.txt", folderPath);
    FILE* file = fopen(detailsPath, "r");
    if (file == NULL) return 0;

    int valid = 0;
    char line[1000];
    while (fgets(line, 1000, file)) {
        trim(line);
        if (strncmp(line, "Judul: ", 7) == 0) {
            char title[MAX_TITLE];
            strncpy(title, line + 7, MAX_TITLE - 1);
            if (strcmp(title, filename) == 0) valid = 1;
            break;
        }
    }
    fclose(file);
    return valid;
}

void deleteStory(char* filename) {
    if (filename == NULL) return;

    char folderPath[MAX_NAME];
    snprintf(folderPath, MAX_NAME, "data/%s", filename);
    char command[300];
    snprintf(command, 300, "rm -rf %s", folderPath);
    system(command);
}