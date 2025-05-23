#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "story.h"


story createStory(char *title, char *description) {
    story S = (story)malloc(sizeof(Story));
    if (S == NULL) {
        printf("gagal\n");
        return NULL;
    }

    strcpy(S->title, title);
    strcpy(S->description, description);
    S->nextStory = NULL;
    S->firstChapter = NULL;
    return S;
}

void addStory(story *s, story storyNew) {
    if (*s == NULL) {
        *s = storyNew;
    } else {
        storyNew->nextStory = *s;
        *s = storyNew;
    }
}

void printAllStory(story s) {
    while (s != NULL) {
        printf("[Judul: %s, Deskripsi: %s] -> ", s->title, s->description);
        s = s->nextStory;
    }
    printf("NULL\n");
}

void writeStoryToFolder(story s) {
    if (s == NULL) return;

    // Buat nama folder dari judul
    char folderName[150];
    snprintf(folderName, sizeof(folderName), "../data/%s", s->title);

    // Buat folder "data" dulu (jika belum)
    mkdir("data", 0777);

    // Buat folder khusus untuk story
    mkdir(folderName, 0777);  // Bisa gagal kalau nama tidak valid (hati-hati dengan karakter aneh)

    // Siapkan path file
    char filePath[200];
    snprintf(filePath, sizeof(filePath), "%s/story.txt", folderName);

    // Simpan file
    FILE *f = fopen(filePath, "w");
    if (f == NULL) {
        printf("Gagal membuka file: %s\n", filePath);
        return;
    }

    fprintf(f, "Judul: %s\n", s->title);
    fprintf(f, "Deskripsi: %s\n", s->description);
    fclose(f);

    printf("Berhasil menyimpan ke: %s\n", filePath);
}