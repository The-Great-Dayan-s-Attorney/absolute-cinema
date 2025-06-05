#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <direct.h>


#include "story.h"
#include "chapter.h"

addressChapter createChapter(const char* title, const char* desc) {
    addressChapter ch = (addressChapter)malloc(sizeof(Chapter));
    if (ch != NULL) {
        strncpy(ch->title, title, MAX_TITLE - 1);
        ch->title[MAX_TITLE - 1] = '\0';

        strncpy(ch->description, desc, MAX_DESCRIPTION - 1);
        ch->description[MAX_DESCRIPTION - 1] = '\0';

        ch->nextChapter = NULL;
        ch->firstScene = NULL;  
    }
    return ch;
}

// delete chapter harusnya

void printChapter(addressChapter ch) {
    if (ch == NULL) {
        printf("Chapter tidak ditemukan.\n");
        return;
    }

    printf("==================================\n");
    printf("Chapter: %s\n", ch->title);
    printf("Description: %s\n", ch->description);
    printf("==================================\n");

    addressScene currentScene = ch->firstScene;
    int sceneIndex = 1;

    while (currentScene != NULL) {
        printf("\nScene %d: %s\n", sceneIndex++, currentScene->title);
        printf("%s\n", currentScene->description);

        // Tampilkan pilihan
        for (int i = 0; i < MAX_CHOICES; i++) {
            if (currentScene->choices[i].id != -1 &&
                currentScene->choices[i].title[0] != '\0' &&
                currentScene->choices[i].nextScene != NULL) {
                
                printf("  Pilihan %d: %s -> %s\n",
                       currentScene->choices[i].id,
                       currentScene->choices[i].title,
                       currentScene->choices[i].nextScene->title);
            }
        }

        currentScene = currentScene->nextScene;
    }

    printf("\n--- Akhir Chapter ---\n");
}

void saveChapterToFile(addressStory s, addressChapter ch) {
    if (s == NULL || ch == NULL) return;

    // Hitung nomor chapter
    int count = 1;
    addressChapter temp = s->chapters.head;
    while (temp != NULL && temp != ch) {
        count++;
        temp = temp->nextChapter;
    }

    char folderPath[150];
    snprintf(folderPath, sizeof(folderPath), "../../data/%s", s->title);
    folderPath[strcspn(folderPath, "\n")] = '\0';

    if (mkdir("data") != 0 && errno != EEXIST) {
        perror("Gagal membuat folder data");
        return;
    }

    if (mkdir(folderPath) != 0 && errno != EEXIST) {
        perror("Gagal membuat folder story");
        return;
    }

    // Path file
    char chapterFile[200];
    snprintf(chapterFile, sizeof(chapterFile), "%s/chapter_%d.txt", folderPath, count);

    FILE *cf = fopen(chapterFile, "w");
    if (cf != NULL) {
        fprintf(cf, "Title: %s", ch->title);
        fprintf(cf, "Description: %s", ch->description);
        fclose(cf);
        printf("Chapter disimpan ke: %s\n", chapterFile);
    } else {
        printf("Gagal menyimpan file chapter ke: %s\n", chapterFile);
    }
}
