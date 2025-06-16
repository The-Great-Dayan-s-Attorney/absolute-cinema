#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "scene.h"
#include "chapter.h"
#include "choices.h"
#include "story.h"


addressScene createScene(const char *title, const char *desc, int id) {
    addressScene sc = (addressScene)malloc(sizeof(Scene));
    if (sc != NULL) {
        sc->id = id;  // id langsung dimasukkan di sini
        strncpy(sc->title, title, MAX_TITLE - 1);
        sc->title[MAX_TITLE - 1] = '\0';

        strncpy(sc->description, desc, MAX_DESCRIPTION - 1);
        sc->description[MAX_DESCRIPTION - 1] = '\0';

        for (int i = 0; i < MAX_CHOICES; i++) {
            sc->choices[i].id = -1;
            sc->choices[i].title[0] = '\0';
            sc->choices[i].nextScene = NULL;
        }
        sc->nextScene = NULL;
    }
    return sc;
}



void addSceneToChapter(addressChapter ch, addressScene sc) {
    if (ch->firstScene == NULL) {
        ch->firstScene = sc;
    } else {
        addressScene temp = ch->firstScene;
        while (temp->nextScene != NULL) {
            temp = temp->nextScene;
        }
        temp->nextScene = sc;
    }
}

addressScene findSceneByID(addressChapter ch, int id) {
    addressScene current = ch->firstScene;
    while (current != NULL) {
        if (current->id == id) {
            return current;  // Ketemu
        }
        current = current->nextScene;
    }
    return NULL;  // Tidak ketemu
}

void printSceneStructure(addressChapter ch) {
    printf("\n=== Struktur Cerita ===\n");
    addressScene sc = ch->firstScene;

    while (sc != NULL) {
        // Bersihkan newline agar rapi
        char cleanTitle[MAX_TITLE];
        strncpy(cleanTitle, sc->title, MAX_TITLE);
        cleanTitle[strcspn(cleanTitle, "\n")] = '\0';

        printf("[Scene %d] %s\n", sc->id, cleanTitle);

        int hasChoice = 0;
        for (int i = 0; i < MAX_CHOICES; i++) {
            if (sc->choices[i].id != -1) {
                hasChoice = 1;
                printf("  > %s --> Scene %d\n", sc->choices[i].title, sc->choices[i].id);
            }
        }
        if (!hasChoice) {
            printf("  (Tidak ada pilihan)\n");
        }

        sc = sc->nextScene;
        printf("\n");
    }
}

void printAllScenes(addressChapter ch) {
    addressScene temp = ch->firstScene;
    while (temp != NULL) {
        printf("\n[Scene ID: %d] %s\n%s\n", temp->id, temp->title, temp->description);
        for (int i = 0; i < MAX_CHOICES; i++) {
            if (temp->choices[i].id != -1) {
                printf("  - %s => Scene ID %d\n", temp->choices[i].title, temp->choices[i].id);
            }
        }
        temp = temp->nextScene;
    }
}

bool hapusSceneDariChapter(addressChapter ch, int sceneID) {
    addressScene prev = NULL;
    addressScene curr = ch->firstScene;

    // Cari scene dengan ID
    while (curr != NULL && curr->id != sceneID) {
        prev = curr;
        curr = curr->nextScene;
    }

    if (curr == NULL) return false; // Tidak ditemukan

    // Hapus referensi ke scene ini dari semua pilihan di scene lain
    addressScene scan = ch->firstScene;
    while (scan != NULL) {
        for (int i = 0; i < MAX_CHOICES; i++) {
            if (scan->choices[i].id == sceneID) {
                scan->choices[i].id = -1;
                scan->choices[i].nextScene = NULL;
                scan->choices[i].title[0] = '\0';
            }
        }
        scan = scan->nextScene;
    }

    // Hapus scene dari linked list
    if (prev == NULL) {
        ch->firstScene = curr->nextScene;
    } else {
        prev->nextScene = curr->nextScene;
    }

    free(curr); // Bebaskan memori
    return true;
}
