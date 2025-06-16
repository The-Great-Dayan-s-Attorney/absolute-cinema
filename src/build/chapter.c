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


addressChapter pilihChapter(Queue *q) {
    if (q->head == NULL) {
        printf("Belum ada chapter.\n");
        return NULL;
    }

    printf("\n=== PILIH CHAPTER ===\n");
    int index = 1;
    addressChapter curr = q->head;
    while (curr != NULL) {
        printf("%d. %s", index, curr->title);  // title sudah ada \n dari fgets
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

void saveChapterWithScenes(addressStory s, addressChapter ch, int chapterIndex) {
    char folderPath[150], filePath[200];
    snprintf(folderPath, sizeof(folderPath), "../../data/%s", s->title);
    snprintf(filePath, sizeof(filePath), "%s/chapter_%d.txt", folderPath, chapterIndex);

    if (mkdir("data") != 0 && errno != EEXIST) {
        perror("Gagal membuat folder data");
        return;
    }

    if (mkdir(folderPath) != 0 && errno != EEXIST) {
        perror("Gagal membuat folder story");
        return;
    }

    FILE *f = fopen(filePath, "w");
    if (!f) return;

    // Simpan judul dan deskripsi
    fprintf(f, "[Judul]\n%s\n\n", ch->title);
    fprintf(f, "[Deskripsi]\n%s\n\n", ch->description);

    // Simpan scene
    fprintf(f, "[Scene]\n");
    addressScene temp = ch->firstScene;
    while (temp != NULL) {
        // Buang newline dari title dan description kalau ada
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
}


addressChapter loadChapterFromFile(const char *filepath) {
    FILE *f = fopen(filepath, "r");
    if (!f) return NULL;

    char line[256];
    addressChapter ch = createChapter("", "");
    addressScene lastScene = NULL;
    addressScene sceneMap[100] = { NULL };

    enum { NONE, JUDUL, DESKRIPSI, SCENE, CHOICES } section = NONE;
    int expectingDesc = 0;          // Flag untuk menunggu baris deskripsi scene
    int currentId = 0;
    char currentTitle[MAX_TITLE] = "";

    while (fgets(line, sizeof(line), f)) {
        // Hilangkan newline di akhir line
        line[strcspn(line, "\n")] = '\0';

        if (strncmp(line, "[Judul]", 7) == 0) {
            section = JUDUL;
            continue;
        } else if (strncmp(line, "[Deskripsi]", 11) == 0) {
            section = DESKRIPSI;
            continue;
        } else if (strncmp(line, "[Scene]", 7) == 0) {
            section = SCENE;
            expectingDesc = 0; // reset flag
            continue;
        } else if (strncmp(line, "[Choices]", 9) == 0) {
            section = CHOICES;
            continue;
        }

            if (section == JUDUL) {
                if (strlen(line) == 0) continue;  // skip baris kosong
                strncpy(ch->title, line, MAX_TITLE);
                ch->title[MAX_TITLE - 1] = '\0';
                printf("DEBUG: Judul chapter = '%s'\n", ch->title);
            } else if (section == DESKRIPSI) {
                if (strlen(line) == 0) continue;  // skip baris kosong
                strncpy(ch->description, line, MAX_DESCRIPTION);
                ch->description[MAX_DESCRIPTION - 1] = '\0';
                printf("DEBUG: Deskripsi chapter = '%s'\n", ch->description);
            } else if (section == SCENE) {
            if (!expectingDesc) {
                // Baris pertama scene: format "ID|Judul"
                if (sscanf(line, "%d|%[^\n]", &currentId, currentTitle) == 2) {
                    expectingDesc = 1; // next line adalah deskripsi
                }
            } else {
                // Baris kedua scene: format "|Deskripsi"
                if (line[0] == '|') {
                    char *desc = line + 1;
                    // Buat scene baru
                    addressScene sc = createScene(currentTitle, desc, currentId);
                    sc->id = currentId;
                    sceneMap[currentId] = sc;
                    if (lastScene) lastScene->nextScene = sc;
                    else ch->firstScene = sc;
                    lastScene = sc;

                    expectingDesc = 0; // reset flag
                }
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
                            strcpy(from->choices[i].title, choiceTitle);
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




void loadChaptersFromFolder(addressStory s) {
    char folderPath[150];
    snprintf(folderPath, sizeof(folderPath), "../../data/%s", s->title);
    folderPath[strcspn(folderPath, "\n")] = '\0';

    DIR *dir = opendir(folderPath);
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, "chapter_") && strstr(entry->d_name, ".txt")) {
            char path[200];
            snprintf(path, sizeof(path), "%s/%s", folderPath, entry->d_name);
            addressChapter ch = loadChapterFromFile(path);
            if (ch != NULL) enqueue(&s->chapters, ch);
        }
    }
    closedir(dir);
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
        // Bersihkan newline
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

void hapusChapter(addressStory s, int index) {
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

    // Update pointer head/tail
    if (prev == NULL) {
        // Menghapus head
        s->chapters.head = curr->nextChapter;
        if (curr == s->chapters.tail) {
            s->chapters.tail = NULL;  // hanya satu elemen
        }
    } else {
        prev->nextChapter = curr->nextChapter;
        if (curr == s->chapters.tail) {
            s->chapters.tail = prev;
        }
    }

    // Hapus file chapter
    char path[200];
    snprintf(path, sizeof(path), "../../data/%s/chapter_%d.txt", s->title, index);
    remove(path);

    // Bebaskan memori
    free(curr);

    printf("Chapter %d berhasil dihapus.\n", index);
}

void resaveAllChapters(addressStory s) {
    // Hapus semua file lama dulu (opsional)
    // system("rm ../../data/StoryName/chapter_*.txt");

    addressChapter curr = s->chapters.head;
    int i = 1;
    while (curr != NULL) {
        saveChapterWithScenes(s, curr, i);
        i++;
        curr = curr->nextChapter;
    }
}


addressChapter loadChapter(addressStory s, int chapterIndex) {
    char filePath[200];
    snprintf(filePath, sizeof(filePath), "../../data/%s/chapter_%d.txt", s->title, chapterIndex);

    FILE *f = fopen(filePath, "r");
    if (!f) {
        printf("Gagal membuka file: %s\n", filePath);
        return NULL;
    }

    addressChapter ch = createChapter("", ""); // placeholder, akan diisi dari file
    char line[256];

    // Baca [Judul]
    fgets(line, sizeof(line), f); // [Judul]
    fgets(ch->title, MAX_TITLE, f); ch->title[strcspn(ch->title, "\n")] = '\0';

    // Baca [Deskripsi]
    fgets(line, sizeof(line), f); // newline kosong
    fgets(line, sizeof(line), f); // [Deskripsi]
    fgets(ch->description, MAX_DESCRIPTION, f); ch->description[strcspn(ch->description, "\n")] = '\0';

    // Baca [Scene]
    fgets(line, sizeof(line), f); // newline kosong
    fgets(line, sizeof(line), f); // [Scene]

    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "[Choices]", 9) == 0) break;

        int id;
        char title[MAX_TITLE], desc[MAX_DESCRIPTION];
        sscanf(line, "%d|%[^|]|%[^\n]", &id, title, desc);

        addressScene sc = createScene(title, desc, id);
        addSceneToChapter(ch, sc);
    }

    // Baca [Choices]
    while (fgets(line, sizeof(line), f)) {
        int fromID, toID;
        char choiceTitle[MAX_TITLE];
        sscanf(line, "%d|%[^|]|%d", &fromID, choiceTitle, &toID);

        addressScene fromScene = findSceneByID(ch, fromID);
        addressScene toScene = findSceneByID(ch, toID);

        if (fromScene && toScene) {
            for (int i = 0; i < MAX_CHOICES; i++) {
                if (fromScene->choices[i].id == -1) {
                    fromScene->choices[i].id = toID;
                    strcpy(fromScene->choices[i].title, choiceTitle);
                    fromScene->choices[i].nextScene = toScene;
                    break;
                }
            }
        }
    }

    fclose(f);
    return ch;
}


