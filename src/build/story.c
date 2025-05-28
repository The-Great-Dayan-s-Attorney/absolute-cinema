#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <direct.h>


#include "story.h"
#include "chapter.h"



addressStory createStory(char *title, char *description) {
    addressStory S = (addressStory)malloc(sizeof(Story));
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

void addStory(addressStory *s, addressStory newStory) {
    if (*s == NULL) {
        *s = newStory;
    } else {
        newStory->nextStory = *s;
        *s = newStory;
    }
}

addressStory findStory(addressStory listStory, const char *titleStory) {
    addressStory current = listStory;
    while (current != NULL) {
        if (strcmp(current->title, titleStory) == 0) {
            return current;   
        }
        current = current->nextStory;
    }
    return NULL;
}

void deleteStoryByTitle(addressStory *headStory, const char *titleStory) {
    addressStory current = *headStory;
    addressStory prev = NULL;

    while (current != NULL && strcmp(current->title, titleStory) != 0) {
        prev = current;
        current = current->nextStory;
    }
    if (current == NULL) {
        printf("Story dengan judul \"%s\" tidak ditemukan.\n", titleStory);
        return;
    }

    deleteAllChapters(current->firstChapter);
    if (prev == NULL) {
        *headStory = current->nextStory;
    } else {
        prev->nextStory = current->nextStory;
    }
    free(current);
    printf("Story dengan judul \"%s\" berhasil dihapus.\n", titleStory);
}


void printAllStory(addressStory s) { // Fungsi pengecekan, bukan mode play
    int index = 1;
    while (s != NULL) {
        printf("Story %d:\n", index++);
        printf("  Judul     : %s\n", s->title);
        printf("  Deskripsi : %s\n", s->description);
        s = s->nextStory;
    }
    printf("NULL\n");
}


void writeStoryToFolder(addressStory s) {
    if (s == NULL) return;

    char folderName[150];
    snprintf(folderName, sizeof(folderName), "../data/%s", s->title);

    mkdir("data");

    mkdir(folderName);  //


    char filePath[200];
    snprintf(filePath, sizeof(filePath), "%s/story.txt", folderName);

  
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