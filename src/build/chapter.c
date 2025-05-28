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

void deleteAllChapters(addressChapter ch) {
    addressChapter temp;
    while (ch != NULL) {
        temp = ch;
        ch = ch->nextChapter;
        free(temp);
    }
}

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

