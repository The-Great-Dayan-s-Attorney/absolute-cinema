#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <direct.h>
#include <errno.h>
#include "filemanager.h"
#include "story.h"
#include "chapter.h"
#include "scene.h"

// Helper untuk menghapus newline
void trim(char* str) {
    size_t len = strlen(str);
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

void fm_save_story(addressStory s) {
    if (s == NULL) return;

    char folderName[150];
    snprintf(folderName, sizeof(folderName), "D:/absolute-cinema/data/%s", s->title);
    folderName[strcspn(folderName, "\n")] = '\0';

    if (mkdir("D:/absolute-cinema/data") != 0 && errno != EEXIST) {
        perror("Gagal membuat folder data");
        fflush(stdout);
        return;
    }

    if (mkdir(folderName) != 0 && errno != EEXIST) {
        perror("Gagal membuat folder story");
        fflush(stdout);
        return;
    }

    char filePath[200];
    snprintf(filePath, sizeof(filePath), "%s/details_story.txt", folderName);

    FILE *f = fopen(filePath, "w");
    if (f == NULL) {
        perror("Gagal membuka file");
        printf("Path: %s\n", filePath);
        fflush(stdout);
        return;
    }

    fprintf(f, "Judul: %s\n", s->title);
    fprintf(f, "Deskripsi: %s\n", s->description);
    fclose(f);

    printf("Berhasil menyimpan ke: %s\n", filePath);
    fflush(stdout);
}

void fm_save_chapter(addressStory s, addressChapter ch) {
    if (s == NULL || ch == NULL) return;

    int count = 1;
    addressChapter temp = s->chapters.head;
    while (temp != NULL && temp != ch) {
        count++;
        temp = temp->nextChapter;
    }

    char folderPath[150];
    snprintf(folderPath, sizeof(folderPath), "D:/absolute-cinema/data/%s", s->title);
    folderPath[strcspn(folderPath, "\n")] = '\0';

    if (mkdir("D:/absolute-cinema/data") != 0 && errno != EEXIST) {
        perror("Gagal membuat folder data");
        fflush(stdout);
        return;
    }

    if (mkdir(folderPath) != 0 && errno != EEXIST) {
        perror("Gagal membuat folder story");
        fflush(stdout);
        return;
    }

    char chapterFile[200];
    snprintf(chapterFile, sizeof(chapterFile), "%s/chapter_%d.txt", folderPath, count);

    FILE *cf = fopen(chapterFile, "w");
    if (cf != NULL) {
        fprintf(cf, "Title: %s", ch->title);
        fprintf(cf, "Description: %s", ch->description);
        fclose(cf);
        printf("Chapter disimpan ke: %s\n", chapterFile);
        fflush(stdout);
    } else {
        printf("Gagal menyimpan file chapter ke: %s\n", chapterFile);
        fflush(stdout);
    }
}

void fm_save_chapter_with_scenes(addressStory s, addressChapter ch, int chapterIndex) {
    char folderPath[150], filePath[200];
    snprintf(folderPath, sizeof(folderPath), "D:/absolute-cinema/data/%s", s->title);
    snprintf(filePath, sizeof(filePath), "%s/chapter_%d.txt", folderPath, chapterIndex);

    if (mkdir("D:/absolute-cinema/data") != 0 && errno != EEXIST) {
        perror("Gagal membuat folder data");
        fflush(stdout);
        return;
    }

    if (mkdir(folderPath) != 0 && errno != EEXIST) {
        perror("Gagal membuat folder story");
        fflush(stdout);
        return;
    }

    FILE *f = fopen(filePath, "w");
    if (!f) {
        printf("Gagal membuka file untuk menyimpan chapter: %s\n", filePath);
        fflush(stdout);
        return;
    }

    fprintf(f, "[Judul]\n%s\n\n", ch->title);
    fprintf(f, "[Deskripsi]\n%s\n\n", ch->description);

    fprintf(f, "[Scene]\n");
    addressScene temp = ch->firstScene;
    while (temp != NULL) {
        temp->title[strcspn(temp->title, "\n")] = '\0';
        temp->description[strcspn(temp->description, "\n")] = '\0';

        fprintf(f, "%d|%s|%s\n", temp->id, temp->title, temp->description);
        temp = temp->nextScene;
    }

    fprintf(f, "\n[Choices]\n");
    temp = ch->firstScene;
    while (temp != NULL) {
        for (int i = 0; i < MAX_CHOICES; i++) {
            if (temp->choices[i].id != -1 && temp->choices[i].nextScene != NULL) {
                fprintf(f, "%d|%s|%d\n", temp->id, temp->choices[i].title, temp->choices[i].nextScene->id);
            }
        }
        temp = temp->nextScene;
    }

    fclose(f);
    printf("Chapter %d berhasil disimpan.\n", chapterIndex);
    fflush(stdout);
}

addressStory fm_load_story(char* filename) {
    if (filename == NULL) return NULL;

    char folderPath[MAX_NAME];
    snprintf(folderPath, MAX_NAME, "D:/absolute-cinema/data/%s", filename);

    char detailsPath[MAX_NAME];
    snprintf(detailsPath, MAX_NAME, "%s/details_story.txt", folderPath);
    FILE* file = fopen(detailsPath, "r");
    if (file == NULL) {
        printf("Gagal membuka %s: %s\n", detailsPath, strerror(errno)); // Debugging dengan errno
        fflush(stdout);
        return NULL;
    }

    char line[1000];
    char storyTitle[MAX_TITLE] = "";
    char storyDesc[MAX_DESCRIPTION] = "";
    while (fgets(line, 1000, file)) {
        trim(line);
        if (strncmp(line, "Judul: ", 7) == 0) strncpy(storyTitle, line + 7, MAX_TITLE - 1);
        else if (strncmp(line, "Deskripsi: ", 11) == 0) strncpy(storyDesc, line + 11, MAX_DESCRIPTION - 1);
    }
    fclose(file);

    if (storyTitle[0] == '\0') {
        printf("Tidak ada judul yang ditemukan di %s\n", detailsPath); // Debugging
        fflush(stdout);
        return NULL;
    }
    addressStory story = createStory(storyTitle, storyDesc);
    if (story == NULL) {
        printf("Gagal membuat story dari %s\n", detailsPath); // Debugging
        fflush(stdout);
        return NULL;
    }

    int chapterNum = 1;
    char chapterPath[MAX_NAME];
    snprintf(chapterPath, MAX_NAME, "%s/chapter_%d.txt", folderPath, chapterNum);
    FILE* chapterFile = fopen(chapterPath, "r");
    addressChapter lastChapter = NULL;

    while (chapterFile != NULL) {
        addressChapter chapter = malloc(sizeof(Chapter));
        if (chapter == NULL) {
            fclose(chapterFile);
            printf("Gagal alokasi memori untuk chapter\n"); // Debugging
            fflush(stdout);
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
                char sceneTitle[MAX_TITLE], sceneDesc[MAX_DESCRIPTION], choices[100], ids[50];
                if (sscanf(line, "Scene: %[^|]|%[^|]|%[^|]|%[^\n]", sceneTitle, sceneDesc, choices, ids) < 4) continue;
                addressScene scene = createScene(sceneTitle, sceneDesc, -1);
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

addressChapter fm_load_chapter_from_file(const char *filepath) {
    FILE *f = fopen(filepath, "r");
    if (!f) {
        perror("Gagal membuka file");
        fflush(stdout);
        return NULL;
    }

    addressChapter ch = createChapter("", "");
    if (!ch) {
        fclose(f);
        printf("Gagal membuat chapter dari %s\n", filepath); // Debugging
        fflush(stdout);
        return NULL;
    }

    char line[256];
    addressScene lastScene = NULL;
    addressScene sceneMap[100] = { NULL };
    enum { NONE, JUDUL, DESKRIPSI, SCENE, CHOICES } section = NONE;

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '\0') continue;

        if (strncmp(line, "[Judul]", 7) == 0) {
            section = JUDUL;
            continue;
        } else if (strncmp(line, "[Deskripsi]", 11) == 0) {
            section = DESKRIPSI;
            continue;
        } else if (strncmp(line, "[Scene]", 7) == 0) {
            section = SCENE;
            continue;
        } else if (strncmp(line, "[Choices]", 9) == 0) {
            section = CHOICES;
            continue;
        }

        if (section == JUDUL) {
            strncpy(ch->title, line, MAX_TITLE - 1);
            ch->title[MAX_TITLE - 1] = '\0';
        } else if (section == DESKRIPSI) {
            strncpy(ch->description, line, MAX_DESCRIPTION - 1);
            ch->description[MAX_DESCRIPTION - 1] = '\0';
        } else if (section == SCENE) {
            int id;
            char title[MAX_TITLE], desc[MAX_DESCRIPTION];
            if (sscanf(line, "%d|%[^|]|%[^\n]", &id, title, desc) == 3) {
                addressScene sc = createScene(title, desc, id);
                if (!sc) continue;
                sceneMap[id] = sc;
                if (lastScene) lastScene->nextScene = sc;
                else ch->firstScene = sc;
                lastScene = sc;
            }
        } else if (section == CHOICES) {
            int fromId, toId;
            char choiceTitle[MAX_TITLE];
            if (sscanf(line, "%d|%[^|]|%d", &fromId, choiceTitle, &toId) == 3) {
                addressScene from = sceneMap[fromId];
                addressScene to = sceneMap[toId];
                if (from && to) {
                    for (int i = 0; i < MAX_CHOICES; i++) {
                        if (from->choices[i].id == -1) {
                            from->choices[i].id = i + 1;
                            strncpy(from->choices[i].title, choiceTitle, MAX_TITLE - 1);
                            from->choices[i].title[MAX_TITLE - 1] = '\0';
                            from->choices[i].nextScene = to;
                            break;
                        }
                    }
                }
            }
        }
    }

    fclose(f);
    return ch;
}

void fm_load_chapters_from_folder(addressStory s) {
    char folderPath[150];
    snprintf(folderPath, sizeof(folderPath), "D:/absolute-cinema/data/%s", s->title);
    folderPath[strcspn(folderPath, "\n")] = '\0';

    DIR *dir = opendir(folderPath);
    if (!dir) {
        printf("Gagal membuka folder %s: %s\n", folderPath, strerror(errno)); // Debugging dengan errno
        fflush(stdout);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, "chapter_") && strstr(entry->d_name, ".txt")) {
            char path[200];
            snprintf(path, sizeof(path), "%s/%s", folderPath, entry->d_name);
            addressChapter ch = fm_load_chapter_from_file(path);
            if (ch != NULL) enqueue(&s->chapters, ch);
        }
    }
    closedir(dir);
}

void fm_load_all_stories(addressStory* listStory) {
    DIR *dir = opendir("D:/absolute-cinema/data");
    if (dir == NULL) {
        printf("Folder tidak ditemukan: D:/absolute-cinema/data: %s\n", strerror(errno)); // Debugging dengan errno
        fflush(stdout);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char folderFullPath[256];
        snprintf(folderFullPath, sizeof(folderFullPath), "D:/absolute-cinema/data/%s", entry->d_name);

        struct stat st;
        if (stat(folderFullPath, &st) == 0 && S_ISDIR(st.st_mode)) {
            char path[300];
            snprintf(path, sizeof(path), "%s/details_story.txt", folderFullPath);

            FILE *f = fopen(path, "r");
            if (f) {
                printf("Membaca file: %s\n", path); // Debugging
                fflush(stdout);
                char line[256];
                char title[MAX_TITLE], desc[MAX_DESCRIPTION];
                int foundTitle = 0;

                while (fgets(line, sizeof(line), f)) {
                    trim(line);
                    if (strncmp(line, "Judul: ", 7) == 0) {
                        strncpy(title, line + 7, MAX_TITLE - 1);
                        title[MAX_TITLE - 1] = '\0';
                        foundTitle = 1;
                    } else if (strncmp(line, "Deskripsi: ", 11) == 0 && foundTitle) {
                        strncpy(desc, line + 11, MAX_DESCRIPTION - 1);
                        desc[MAX_DESCRIPTION - 1] = '\0';
                        break;
                    }
                }
                fclose(f);

                if (foundTitle) {
                    addressStory s = createStory(title, desc);
                    if (s != NULL) {
                        addStory(listStory, s);
                        printf("Cerita ditemukan: %s\n", title); // Debugging
                        fflush(stdout);
                    }
                } else {
                    printf("Tidak ada judul yang valid di %s\n", path); // Debugging
                    fflush(stdout);
                }
            } else {
                printf("Gagal membuka %s: %s\n", path, strerror(errno)); // Debugging dengan errno
                fflush(stdout);
            }
        } else {
            printf("Bukan direktori atau error stat: %s\n", folderFullPath); // Debugging
            fflush(stdout);
        }
    }

    closedir(dir);
}

addressChapter fm_load_chapter(addressStory s, int chapterIndex) {
    char filePath[200];
    snprintf(filePath, sizeof(filePath), "D:/absolute-cinema/data/%s/chapter_%d.txt", s->title, chapterIndex);

    return fm_load_chapter_from_file(filePath);
}

void fm_resave_all_chapters(addressStory s) {
    addressChapter curr = s->chapters.head;
    int i = 1;
    while (curr != NULL) {
        fm_save_chapter_with_scenes(s, curr, i);
        i++;
        curr = curr->nextChapter;
    }
}

StoryEntry* listStories(int* count) {
    *count = 0;
    StoryEntry* stories = malloc(MAX_STORY * sizeof(StoryEntry));
    if (stories == NULL) {
        printf("Gagal alokasi memori untuk stories\n"); // Debugging
        fflush(stdout);
        return NULL;
    }

    DIR* dir = opendir("D:/absolute-cinema/data");
    if (dir == NULL) {
        printf("Gagal membuka folder data: D:/absolute-cinema/data: %s\n", strerror(errno)); // Debugging dengan errno
        fflush(stdout);
        free(stories);
        return NULL;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char folderPath[MAX_NAME];
        snprintf(folderPath, MAX_NAME, "D:/absolute-cinema/data/%s", entry->d_name);

        struct stat st;
        if (stat(folderPath, &st) == 0 && S_ISDIR(st.st_mode)) {
            char detailsPath[MAX_NAME];
            snprintf(detailsPath, MAX_NAME, "%s/details_story.txt", folderPath);
            FILE* file = fopen(detailsPath, "r");
            if (file != NULL) {
                printf("Membaca: %s\n", detailsPath); // Debugging
                fflush(stdout);
                char line[1000];
                while (fgets(line, 1000, file)) {
                    trim(line);
                    if (strncmp(line, "Judul: ", 7) == 0) {
                        strncpy(stories[*count].filename, entry->d_name, MAX_NAME - 1);
                        stories[*count].filename[MAX_NAME - 1] = '\0';
                        strncpy(stories[*count].title, line + 7, MAX_TITLE - 1);
                        stories[*count].title[MAX_TITLE - 1] = '\0';
                        (*count)++;
                        break;
                    }
                }
                fclose(file);
            } else {
                printf("Gagal membuka %s: %s\n", detailsPath, strerror(errno)); // Debugging dengan errno
                fflush(stdout);
            }
        } else {
            printf("Bukan direktori atau error stat: %s\n", folderPath); // Debugging
            fflush(stdout);
        }
    }

    closedir(dir);
    if (*count == 0) {
        printf("Tidak ada cerita yang ditemukan di D:/absolute-cinema/data\n"); // Debugging
        fflush(stdout);
        free(stories);
        return NULL;
    }
    return stories;
}

int validateFile(char* filename) {
    if (filename == NULL) return 0;

    char folderPath[MAX_NAME];
    snprintf(folderPath, MAX_NAME, "D:/absolute-cinema/data/%s", filename);
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
    snprintf(folderPath, MAX_NAME, "D:/absolute-cinema/data/%s", filename);
    char command[300];
    snprintf(command, 300, "rm -rf %s", folderPath);
    system(command);
}

void updateStoryFile(addressStory story, char* filename) {
    if (story == NULL || filename == NULL) return;

    char folderPath[MAX_NAME];
    snprintf(folderPath, MAX_NAME, "D:/absolute-cinema/data/%s", filename);

    char detailsPath[MAX_NAME];
    snprintf(detailsPath, MAX_NAME, "%s/details_story.txt", folderPath);

    FILE* file = fopen(detailsPath, "w");
    if (file == NULL) {
        printf("Gagal membuka file untuk diperbarui!\n");
        fflush(stdout);
        return;
    }

    fprintf(file, "Judul: %s\n", story->title);
    fprintf(file, "Deskripsi: %s\n", story->description);
    fclose(file);

    printf("File story berhasil diperbarui!\n");
    fflush(stdout);
}