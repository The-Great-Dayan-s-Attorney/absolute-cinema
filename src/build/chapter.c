#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <direct.h>
#include <dirent.h>
#include <errno.h>

#include "story.h"
#include "chapter.h"
#include "filemanager.h"

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

addressChapter pilihChapter(Queue *q) {
    if (q->head == NULL) {
        printf("Belum ada chapter.\n");
        return NULL;
    }

    printf("\n=== PILIH CHAPTER ===\n");
    int index = 1;
    addressChapter curr = q->head;
    while (curr != NULL) {
        printf("%d. %s", index, curr->title);
        curr = curr->nextChapter;
        index++;
    }

    int pilihan;
    printf("Masukkan nomor chapter: ");
    scanf("%d", &pilihan); getchar();

    if (pilihan < 1 || pilihan >= index) {
        printf("Pilihan tidak valid.\n");
        return NULL;
    }

    curr = q->head;
    for (int i = 1; i < pilihan; i++) {
        curr = curr->nextChapter;
    }

    return curr;
}

void printAllChapter(const Queue *q) {
    if (isQueueEmpty(*q)) {
        printf("\nBelum ada chapter yang tersedia.\n");
        return;
    }

    addressChapter current = q->head;
    int index = 1;

    printf("\n=== DAFTAR CHAPTER ===\n\n");

    while (current != NULL) {
        char titleClean[MAX_TITLE];
        char descClean[MAX_DESCRIPTION];
        strncpy(titleClean, current->title, MAX_TITLE);
        strncpy(descClean, current->description, MAX_DESCRIPTION);
        titleClean[strcspn(titleClean, "\n")] = '\0';
        descClean[strcspn(descClean, "\n")] = '\0';

        printf("+----------------------------------------+\n");
        printf("| Chapter %2d: %-26s |\n", index, titleClean);
        printf("|----------------------------------------|\n");
        printf("| %-38s |\n", descClean);
        printf("+----------------------------------------+\n\n");

        current = current->nextChapter;
        index++;
    }
}

void deleteChapter(addressStory s, int index) {
    if (index < 1 || isQueueEmpty(s->chapters)) {
        printf("Index tidak valid atau queue kosong.\n");
        return;
    }

    addressChapter curr = s->chapters.head;
    addressChapter prev = NULL;

    int currentIndex = 1;
    while (curr != NULL && currentIndex < index) {
        prev = curr;
        curr = curr->nextChapter;
        currentIndex++;
    }

    if (curr == NULL) {
        printf("Chapter tidak ditemukan.\n");
        return;
    }

    if (prev == NULL) {
        s->chapters.head = curr->nextChapter;
        if (curr == s->chapters.tail) {
            s->chapters.tail = NULL;
        }
    } else {
        prev->nextChapter = curr->nextChapter;
        if (curr == s->chapters.tail) {
            s->chapters.tail = prev;
        }
    }

    char path[200];
    snprintf(path, sizeof(path), "../../data/%s/chapter_%d.txt", s->title, index);
    remove(path);

    free(curr);

    printf("Chapter %d berhasil dihapus.\n", index);
}

int getChapterCount(Queue *q) {
    int count = 0;
    addressChapter curr = q->head;
    while (curr != NULL) {
        count++;
        curr = curr->nextChapter;
    }
    return count;
}