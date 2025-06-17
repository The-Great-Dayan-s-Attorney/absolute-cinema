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
                char* storyName = pilihStoryDariFolder("../../data");

                if (storyName != NULL) {
                    char storyPath[200];
                    snprintf(storyPath, sizeof(storyPath), "../../data/%s/details_story.txt", storyName);

                    FILE *f = fopen(storyPath, "r");
                    if (f == NULL) {
                        printf("Gagal membuka file: %s\n", storyPath);
                    } else {
                        char line[256];
                        char title[100], desc[200];

                        fgets(line, sizeof(line), f);
                        sscanf(line, "Judul: %[^\n]", title);

                        fgets(line, sizeof(line), f);
                        sscanf(line, "Deskripsi: %[^\n]", desc);

                        fclose(f);

                        addressStory s = createStory(title, desc);
                        addStory(&listStory, s);

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
                char title[MAX_TITLE], desc[MAX_DESCRIPTION];
                printf("Judul Chapter: "); fgets(title, MAX_TITLE, stdin);
                printf("Deskripsi Chapter: "); fgets(desc, MAX_DESCRIPTION, stdin);

                addressChapter ch = createChapter(title, desc);
                enqueue(&s->chapters, ch);

                int index = getChapterCount(&s->chapters);
                fm_save_chapter_with_scenes(s, ch, index);
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
                addressChapter curr = s->chapters.head;
                if (curr == NULL) {
                    printf("Belum ada chapter.\n");
                    break;
                }

                printf("\n=== PILIH CHAPTER ===\n");
                int index = 1;
                while (curr != NULL) {
                    printf("%d. %s\n", index, curr->title);
                    curr = curr->nextChapter;
                    index++;
                }

                int pilihChapter;
                printf("Masukkan nomor chapter: ");
                scanf("%d", &pilihChapter); getchar();

                if (pilihChapter < 1 || pilihChapter >= index) {
                    printf("Pilihan tidak valid.\n");
                    break;
                }

                addressChapter ch = fm_load_chapter(s, pilihChapter);
                if (ch != NULL) {
                    menuChapter(s, ch, pilihChapter);
                } else {
                    printf("Gagal memuat chapter.\n");
                }

                break;
            }

            case 4: {
                fm_resave_all_chapters(s);
                printf("Semua chapter berhasil disimpan ulang.\n");
                break;
            }
            case 5: {
                if (isQueueEmpty(s->chapters)) {
                    printf("Belum ada chapter untuk diedit.\n");
                    break;
                }

                printAllChapter(&s->chapters);
                int pilih;
                printf("Pilih nomor chapter yang ingin diedit: ");
                scanf("%d", &pilih); getchar();

                addressChapter curr = s->chapters.head;
                for (int i = 1; i < pilih && curr != NULL; i++) {
                    curr = curr->nextChapter;
                }

                if (curr == NULL) {
                    printf("Chapter tidak ditemukan.\n");
                    break;
                }

                printf("Judul baru (kosongkan untuk tidak mengubah): ");
                char newTitle[MAX_TITLE];
                fgets(newTitle, MAX_TITLE, stdin);

                printf("Deskripsi baru (kosongkan untuk tidak mengubah): ");
                char newDesc[MAX_DESCRIPTION];
                fgets(newDesc, MAX_DESCRIPTION, stdin);

                if (strlen(newTitle) > 1)
                    strncpy(curr->title, newTitle, MAX_TITLE);
                if (strlen(newDesc) > 1)
                    strncpy(curr->description, newDesc, MAX_DESCRIPTION);

                int index = getChapterCount(&s->chapters);
                fm_save_chapter_with_scenes(s, curr, index);

                printf("Chapter berhasil diperbarui.\n");
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
                char judul[MAX_TITLE], deskripsi[MAX_DESCRIPTION];
                printf("Judul Scene: "); fgets(judul, MAX_TITLE, stdin);
                printf("Deskripsi Scene: "); fgets(deskripsi, MAX_DESCRIPTION, stdin);

                addressScene sc = createScene(judul, deskripsi, sceneCounter++);
                addSceneToChapter(ch, sc);
                printf("Scene #%d ditambahkan.\n", sc->id);

                char tambahChoice = 'y';
                while (tambahChoice == 'y' || tambahChoice == 'Y') {
                    for (int i = 0; i < MAX_CHOICES; i++) {
                        if (sc->choices[i].id == -1) {
                            printf("\n-- Pilihan #%d --\n", i + 1);
                            printf("Judul Pilihan: ");
                            fgets(sc->choices[i].title, MAX_TITLE, stdin);
                            sc->choices[i].title[strcspn(sc->choices[i].title, "\n")] = '\0';

                            char tujuanTitle[MAX_TITLE], tujuanDesc[MAX_DESCRIPTION];
                            printf("Scene Tujuan - Judul: ");
                            fgets(tujuanTitle, MAX_TITLE, stdin);
                            printf("Scene Tujuan - Deskripsi: ");
                            fgets(tujuanDesc, MAX_DESCRIPTION, stdin);

                            addressScene tujuanScene = createScene(tujuanTitle, tujuanDesc, sceneCounter++);
                            addSceneToChapter(ch, tujuanScene);

                            sc->choices[i].id = tujuanScene->id;
                            sc->choices[i].nextScene = tujuanScene;

                            printf("Scene Tujuan #%d ditambahkan & terhubung.\n", tujuanScene->id);

                            printf("Tambah pilihan lagi? (y/n): ");
                            scanf(" %c", &tambahChoice); getchar();
                            break;
                        }
                    }
                }
                break;
            }

            case 2: {
                printSceneStructure(ch);
                int fromID, toID;
                printf("ID Scene asal: ");
                scanf("%d", &fromID); getchar();
                addressScene fromScene = findSceneByID(ch, fromID);
                if (!fromScene) {
                    printf("Scene asal tidak ditemukan.\n");
                    break;
                }

                for (int i = 0; i < MAX_CHOICES; i++) {
                    if (fromScene->choices[i].id == -1) {
                        printf("Judul Pilihan: ");
                        fgets(fromScene->choices[i].title, MAX_TITLE, stdin);
                        fromScene->choices[i].title[strcspn(fromScene->choices[i].title, "\n")] = '\0';

                        printf("ID Scene tujuan (yang sudah ada): ");
                        scanf("%d", &toID); getchar();

                        addressScene toScene = findSceneByID(ch, toID);
                        if (!toScene) {
                            printf("Scene tujuan tidak ditemukan.\n");
                            break;
                        }

                        fromScene->choices[i].id = toID;
                        fromScene->choices[i].nextScene = toScene;
                        printf("Pilihan berhasil ditambahkan ke Scene %d.\n", fromID);
                        break;
                    }
                }
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
                printAllScenes(ch);
                int sceneID;

                printf("Masukkan ID Scene yang ingin diedit: ");
                scanf("%d", &sceneID); getchar();

                addressScene scene = findSceneByID(ch, sceneID);
                if (scene == NULL) {
                    printf("Scene tidak ditemukan.\n");
                    break;
                }

                printf("Judul lama: %s", scene->title);
                printf("Deskripsi lama: %s", scene->description);

                printf("Masukkan judul baru (atau tekan ENTER untuk tidak mengubah): ");
                char newTitle[MAX_TITLE];
                fgets(newTitle, MAX_TITLE, stdin);
                if (newTitle[0] != '\n') {
                    strncpy(scene->title, newTitle, MAX_TITLE);
                }

                printf("Masukkan deskripsi baru (atau tekan ENTER untuk tidak mengubah): ");
                char newDesc[MAX_DESCRIPTION];
                fgets(newDesc, MAX_DESCRIPTION, stdin);
                if (newDesc[0] != '\n') {
                    strncpy(scene->description, newDesc, MAX_DESCRIPTION);
                }

                printf("Scene berhasil diperbarui.\n");
                break;
            }
            case 7: {
                printAllScenes(ch);
                int sceneID;
                printf("Masukkan ID Scene yang ingin dihapus: ");
                scanf("%d", &sceneID); getchar();

                bool success = hapusSceneDariChapter(ch, sceneID);
                if (success) {
                    printf("Scene dengan ID %d berhasil dihapus.\n", sceneID);
                } else {
                    printf("Scene tidak ditemukan atau gagal dihapus.\n");
                }
                break;
            }

        }
    } while (pilihan != 8);
}