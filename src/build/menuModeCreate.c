#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "story.h"
#include "chapter.h"
#include "queue.h"
#include "scene.h"
#include "choices.h"
#include "filemanager.h"

void menuStory(addressStory s);

void menuChapter(addressStory s, addressChapter ch, int chapterIndex);

void mainMenuBuild() {
    addressStory listStory = NULL;
    fm_load_all_stories(&listStory);
    int pilihan;

    do {
        printf("\n=== MENU BUILD GAME ===\n");
        printf("1. Tambah Story\n");
        printf("2. Pilih Story\n");
        printf("3. Lihat Semua Story\n");
        printf("4. Simpan Semua Story ke File\n");
        printf("5. Edit Story\n");
        printf("6. Delete Story\n");
        printf("7. Keluar\n");
        printf("Pilihan: ");
        scanf("%d", &pilihan); getchar();

        switch (pilihan) {
            case 1: {
                char title[100], desc[200];
                printf("Judul: "); fgets(title, 100, stdin);
                printf("Deskripsi: "); fgets(desc, 200, stdin);
                addressStory s = createStory(title, desc);
                addStory(&listStory, s);

                fm_save_story(s);
                break;
            }
            case 2: {
                pilihDanBukaStory(&listStory);
                break;
            }

            case 3:
                printAllStory(listStory);
                break;
            case 4: {
                addressStory curr = listStory;
                while (curr) {
                    fm_save_story(curr);
                    fm_resave_all_chapters(curr);
                    curr = curr->nextStory;
                }
                break;
            }
            case 5: {
                editStory(listStory);
                break;
            }
            case 6: {
                hapusStory(&listStory);
                break;
            }
        }
    } while (pilihan != 7);
}

void menuStory(addressStory s) {
    static int loaded = 0;

    if (!loaded) {
        fm_load_chapters_from_folder(s);
        loaded = 1;
    }

    int pilihan;
    do {
        printf("\n=== STORY: %s ===\n", s->title);
        printf("1. Tambah Chapter\n");
        printf("2. Lihat Chapter\n");
        printf("3. Pilih Chapter\n");
        printf("4. Simpan Story ke Folder\n");
        printf("5. Edit Chapter\n");
        printf("6. Hapus Chapter\n");
        printf("7. Kembali\n");

        printf("Pilihan: ");
        scanf("%d", &pilihan); getchar();

        switch (pilihan) {
            case 1: {
                tambahChapter(s);
                break;
            }

            case 2: {
                if (s->chapters.head == NULL) {
                    printf("Belum ada chapter pada story ini.\n");
                } else {
                    printAllChapter(&s->chapters);
                }
                break;
            }

            case 3: {
                pilihChapterDariStory(s);
                break;
            }

            case 4: {
                fm_resave_all_chapters(s);
                printf("Semua chapter berhasil disimpan ulang.\n");
                break;
            }
            case 5: {
                editChapterDariStory(s);
                break;
            }
            case 6: {
                printAllChapter(&s->chapters);
                printf("Masukkan nomor chapter yang ingin dihapus: ");
                int idx;
                scanf("%d", &idx); getchar();
                deleteChapter(s, idx);
                break;
            }

        }
    } while (pilihan != 7);
}

void menuChapter(addressStory s, addressChapter ch, int chapterIndex) {
    int pilihan;
    int sceneCounter = 1;
    do {
        printf("\n=== CHAPTER: %s ===\n", ch->title);
        printf("1. Tambah Scene Baru Sekaligus Pilihan\n");
        printf("2. Tambah Pilihan Menuju Scene yang Sudah Ada\n");
        printf("3. Lihat Semua Scene\n");
        printf("4. Simpan Chapter ke File\n");
        printf("5. Lihat Struktur Cerita\n");
        printf("6. Edit Scene\n");
        printf("7. Hapus Scene\n");
        printf("8. Kembali\n");
        printf("Pilihan: ");
        scanf("%d", &pilihan); getchar();

        switch (pilihan) {
            case 1: {
                chapterTambahSceneBaru(s, ch, &sceneCounter);
                break;
            }

            case 2: {
                chapterTambahPilihanKeScene(ch);
                break;
            }

            case 3: {
                printAllScenes(ch);
            }
            case 4:
                fm_save_chapter_with_scenes(s, ch, chapterIndex);
                break;
            case 5:
                printSceneStructure(ch);
                break;
            case 6: {
                chapterEditScene(ch);
                break;
            }
            case 7: {
                chapterHapusScene(ch);
                break;
            }

        }
    } while (pilihan != 8);
}