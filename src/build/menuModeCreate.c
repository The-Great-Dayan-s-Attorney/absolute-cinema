#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "story.h"
#include "chapter.h"
#include "queue.h"
#include "scene.h"
#include "choices.h"

#include "story.c"
#include "chapter.c"
#include "queue.c"
#include "scene.c"

void menuStory(addressStory s);

void mainMenuBuild() {
    addressStory listStory = NULL;
    int pilihan;

    do {
        printf("\n=== MENU BUILD GAME ===\n");
        printf("1. Tambah Story\n");
        printf("2. Pilih Story\n");
        printf("3. Lihat Semua Story\n");
        printf("4. Simpan Semua Story ke File\n");
        printf("5. Keluar\n");
        printf("Pilihan: ");
        scanf("%d", &pilihan); getchar();

        switch (pilihan) {
            case 1: {
                char title[100], desc[200];
                printf("Judul: "); fgets(title, 100, stdin);
                printf("Deskripsi: "); fgets(desc, 200, stdin);
                addressStory s = createStory(title, desc);
                addStory(&listStory, s);

                writeStoryToFolder(s);
                break;
            }
            case 2: {
                char* storyName = pilihStoryDariFolder("../../data");

                if (storyName != NULL) {
                    // Path yang benar ke file story.txt
                    char storyPath[200];
                    snprintf(storyPath, sizeof(storyPath), "../../data/%s/details_story.txt", storyName);

                    FILE *f = fopen(storyPath, "r");
                    if (f == NULL) {
                        printf("Gagal membuka file: %s\n", storyPath);
                    } else {
                        char line[256];
                        char title[100], desc[200];

                        // Asumsikan format di file:
                        // Judul: ...
                        // Deskripsi: ...

                        fgets(line, sizeof(line), f);  // Judul
                        sscanf(line, "Judul: %[^\n]", title);

                        fgets(line, sizeof(line), f);  // Deskripsi
                        sscanf(line, "Deskripsi: %[^\n]", desc);

                        fclose(f);

                        // Buat addressStory dari file
                        addressStory s = createStory(title, desc);
                        // Kalau kamu mau, bisa tambahkan ke listStory juga
                        addStory(&listStory, s);

                        // Masuk ke menu story
                        menuStory(s);
                    }
                } else {
                    printf("Tidak ada story yang dipilih.\n");
                }

                break;
            }

            case 3:
                printAllStory(listStory);
                break;
            case 4: {
                addressStory curr = listStory;
                while (curr) {
                    writeStoryToFolder(curr);
                    curr = curr->nextStory;
                }
                break;
            }
        }
    } while (pilihan != 5);
}

void menuStory(addressStory s) {
    int pilihan;
    do {
        printf("\n=== STORY: %s ===\n", s->title);
        printf("1. Tambah Chapter\n");
        printf("2. Lihat Chapter\n");
        printf("3. Pilih Chapter\n");
        printf("4. Simpan Story ke Folder\n");
        printf("5. Kembali\n");
        printf("Pilihan: ");
        scanf("%d", &pilihan); getchar();

        switch (pilihan) {
            case 1: {
                char title[MAX_TITLE], desc[MAX_DESCRIPTION];
                printf("Judul Chapter: "); fgets(title, MAX_TITLE, stdin);
                printf("Deskripsi Chapter: "); fgets(desc, MAX_DESCRIPTION, stdin);

                addressChapter ch = createChapter(title, desc);
                enqueue(&s->chapters, ch);
                saveChapterToFile(s, ch);
                break;
            }
            case 2:
                printQueue(&s->chapters);
                break;
            case 3: {
                printf("cas 3");
                break;
            }
            case 4:
                writeStoryToFolder(s);
                break;
        }
    } while (pilihan != 5);
}
