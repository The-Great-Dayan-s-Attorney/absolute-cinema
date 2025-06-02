#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "filemanager.h"
#include "story.h"
#include "chapter.h"
#include "scene.h"
#include "choices.h"

FileManager* createFileManager() {
    FileManager* fm = malloc(sizeof(FileManager));
    if (fm == NULL) {
        fprintf(stderr, "Error: Gagal mengalokasi memori!\n");
        return NULL;
    }
    fm->dummy = 0;
    return fm;
}

void saveStory(addressStory story, char* filename) {
    if (story == NULL || filename == NULL) {
        fprintf(stderr, "Error: Story belum terdaftar!\n");
        return;
    }

    char filepath[MAX_NAME];
    snprintf(filepath, MAX_NAME, "data/Cerita 1/%s", filename);

    FILE* file = fopen(filepath, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Gagal membuat cerita! %s\n", filepath);
        return;
    }

    fprintf(file, "Story: %d|%s|%s\n", story->id, story->title, story->description);

    addressChapter chapter = story->firstChapter;
    while (chapter != NULL) {
        fprintf(file, "Chapter: %s|%s\n", chapter->title, chapter->description);
        addressScene scene = chapter->firstScene;
        while (scene != NULL) {
            char choices[500] = "";
            char ids[50] = "";
            for (int i = 0; i < MAX_CHOICES; i++) {
                if (scene->choices[i].title[0] != '\0') {
                    if (i > 0) {
                        strcat(choices, ",");
                        strcat(ids, ",");
                    }
                    strcat(choices, scene->choices[i].title);
                    char id_str[10];
                    snprintf(id_str, 10, "%d", scene->choices[i].id);
                    strcat(ids, id_str);
                }
            }
            char* nextStory = (story->nextStory == NULL) ? "" : story->nextStory->title;
            fprintf(file, "Scene: %s|%s|%s|%s|%s\n",
                    scene->title, scene->description, choices, ids, nextStory);
            scene = scene->nextScene;
        }
        chapter = chapter->nextChapter;
    }

    fclose(file);
}

addressStory loadStory(char* filename) {
    if (!validateFile(filename)) {
        fprintf(stderr, "Error: File invalid!\n");
        return NULL;
    }

    char filepath[MAX_NAME];
    snprintf(filepath, MAX_NAME, "data/Cerita 1/%s", filename);

    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Gagal memuat cerita! %s\n", filepath);
        return NULL;
    }

    addressStory story = NULL;
    addressChapter lastChapter = NULL;
    addressScene lastScene = NULL;
    char line[1000];

    addressScene scenes[100];
    int sceneCount = 0;
    int choiceIds[100][MAX_CHOICES];
    int choiceCounts[100] = {0};

    while (fgets(line, 1000, file)) {
        if (strncmp(line, "Story: ", 7) == 0) {
            int id;
            char title[MAX_TITLE], desc[MAX_DESCRIPTION];
            if (sscanf(line, "Story: %d|%[^|]|%[^\n]", &id, title, desc) == 3) {
                story = createStory(title, desc);
                story->id = id;
            }
        } else if (strncmp(line, "Chapter: ", 9) == 0) {
            char title[MAX_TITLE], desc[MAX_DESCRIPTION];
            if (sscanf(line, "Chapter: %[^|]|%[^\n]", title, desc) == 2) {
                addressChapter chapter = malloc(sizeof(Chapter));
                if (chapter) {
                    strncpy(chapter->title, title, MAX_TITLE - 1);
                    chapter->title[MAX_TITLE - 1] = '\0';
                    strncpy(chapter->description, desc, MAX_DESCRIPTION - 1);
                    chapter->description[MAX_DESCRIPTION - 1] = '\0';
                    chapter->nextChapter = NULL;
                    chapter->firstScene = NULL;
                    if (story->firstChapter == NULL) {
                        story->firstChapter = chapter;
                    } else {
                        lastChapter->nextChapter = chapter;
                    }
                    lastChapter = chapter;
                }
            }
        } else if (strncmp(line, "Scene: ", 7) == 0) {
            char title[MAX_TITLE], desc[MAX_DESCRIPTION], choices[500], ids[50], nextStory[MAX_TITLE];
            if (sscanf(line, "Scene: %[^|]|%[^|]|%[^|]|%[^|]|%[^\n]",
                       title, desc, choices, ids, nextStory) >= 4) {
                addressScene scene = createScene(title, desc);
                if (scene) {
                    char* choice = strtok(choices, ",");
                    char* id = strtok(ids, ",");
                    int i = 0;
                    while (choice && id && i < MAX_CHOICES) {
                        strncpy(scene->choices[i].title, choice, MAX_TITLE - 1);
                        scene->choices[i].title[MAX_TITLE - 1] = '\0';
                        choiceIds[sceneCount][i] = atoi(id);
                        i++;
                        choice = strtok(NULL, ",");
                        id = strtok(NULL, ",");
                    }
                    choiceCounts[sceneCount] = i;
                    addSceneToChapter(lastChapter, scene);
                    scenes[sceneCount++] = scene;
                    if (nextStory[0] != '\0') {
                        story->nextStory = createStory(nextStory, "");
                    }
                }
            }
        }
    }

    fclose(file);

    for (int i = 0; i < sceneCount; i++) {
        for (int j = 0; j < choiceCounts[i]; j++) {
            int id = choiceIds[i][j];
            for (int k = 0; k < sceneCount; k++) {
                for (int m = 0; m < choiceCounts[k]; m++) {
                    if (scenes[k]->choices[m].id == id) {
                        scenes[i]->choices[j].nextScene = scenes[k];
                        break;
                    }
                }
            }
        }
    }

    return story;
}

StoryEntry* listStories(int* count) {
    *count = 0;
    StoryEntry* stories = malloc(MAX_STORY * sizeof(StoryEntry));
    if (stories == NULL) {
        fprintf(stderr, "Error: Gagal mengalokasi memori!\n");
        return NULL;
    }

    const char* dirs[] = {"data/Cerita 1", "data/Cerita 2"};
    for (int d = 0; d < 2; d++) {
        DIR* dir = opendir(dirs[d]);
        if (dir == NULL) {
            fprintf(stderr, "Error: Tidak bisa dibuka! %s\n", dirs[d]);
            continue;
        }
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strstr(entry->d_name, ".txt")) {
                char filepath[MAX_NAME];
                snprintf(filepath, MAX_NAME, "%s/%s", dirs[d], entry->d_name);
                if (validateFile(filepath)) {
                    FILE* file = fopen(filepath, "r");
                    if (file) {
                        char line[1000];
                        if (fgets(line, 1000, file)) {
                            char title[MAX_TITLE];
                            if (sscanf(line, "Story: %*d|%[^|]", title) == 1) {
                                strncpy(stories[*count].filename, entry->d_name, MAX_NAME - 1);
                                stories[*count].filename[MAX_NAME - 1] = '\0';
                                strncpy(stories[*count].title, title, MAX_TITLE - 1);
                                stories[*count].title[MAX_TITLE - 1] = '\0';
                                (*count)++;
                            }
                        }
                        fclose(file);
                    }
                }
            }
        }
        closedir(dir);
    }

    return stories;
}

int validateFile(char* filename) {
    if (filename == NULL) {
        return 0;
    }
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return 0;
    }
    char line[1000];
    if (fgets(line, 1000, file)) {
        if (strncmp(line, "Story: ", 7) == 0) {
            char id[10], title[MAX_TITLE], desc[MAX_DESCRIPTION];
            if (sscanf(line, "Story: %[^|]|%[^|]|%[^\n]", id, title, desc) == 3) {
                fclose(file);
                return 1;
            }
        }
    }
    fclose(file);
    return 0;
}

void deleteStory(char* filename) {
    if (filename == NULL) {
        fprintf(stderr, "Error: Story belum terdaftar\n");
        return;
    }
    char filepath[MAX_NAME];
    snprintf(filepath, MAX_NAME, "data/Cerita 1/%s", filename);
    if (!validateFile(filepath)) {
        fprintf(stderr, "Error: Story tidak ditemukan!\n");
        return;
    }
    if (remove(filepath) != 0) {
        fprintf(stderr, "Error: Story tidak bisa dihapus! %s\n", filepath);
    } else {
        printf("Story berhasil dihapus!\n");
    }
}