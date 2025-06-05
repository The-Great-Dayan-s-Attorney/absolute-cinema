#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <direct.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>



#include "story.h"
#include "chapter.h"
#include "convention.h"
#include "queue.h"


addressStory createStory(char *title, char *description) {
    addressStory S = (addressStory)malloc(sizeof(Story));
    if (S == NULL) {
        printf("gagal\n");
        return NULL;
    }

    strcpy(S->title, title);
    strcpy(S->description, description);
    S->nextStory = NULL;
    initQueue(&S->chapters);  // Inisialisasi queue chapter
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

void deleteAllChapters(Queue* q) {
    addressChapter temp;
    while (q->head != NULL) {
        temp = q->head;
        q->head = q->head->nextChapter;
        free(temp);
    }
    q->tail = NULL;
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

    // Hapus semua chapter dari queue
    deleteAllChapters(&current->chapters);

    // Hapus node story dari linked list
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


#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>

void writeStoryToFolder(addressStory s) {
    if (s == NULL) return;

    char folderName[150];
    snprintf(folderName, sizeof(folderName), "../../data/%s", s->title);
    folderName[strcspn(folderName, "\n")] = '\0';

    if (mkdir("data") != 0 && errno != EEXIST) {
        perror("Gagal membuat folder data");
        return;
    }

    if (mkdir(folderName) != 0 && errno != EEXIST) {
        perror("Gagal membuat folder story");
        return;
    }

    char filePath[200];
    snprintf(filePath, sizeof(filePath), "%s/details_story.txt", folderName);

    FILE *f = fopen(filePath, "w");
    if (f == NULL) {
        perror("Gagal membuka file");
        printf("Path: %s\n", filePath);
        return;
    }

    fprintf(f, "Judul: %s\n", s->title);
    fprintf(f, "Deskripsi: %s\n", s->description);
    fclose(f);

    printf("Berhasil menyimpan ke: %s\n", filePath);
}


char* pilihStoryDariFolder(const char* basePath) {
    DIR *dir;
    struct dirent *entry;

    static char storyNames[MAX_STORY][MAX_NAME];
    int count = 0;

    dir = opendir(basePath);
    if (dir == NULL) {
        perror("Tidak bisa membuka direktori");
        return NULL;
    }

    printf("\n=== PILIH STORY DARI '%s' ===\n", basePath);
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            printf("%d. %s\n", count + 1, entry->d_name);
            strncpy(storyNames[count], entry->d_name, MAX_NAME);
            count++;
        }
    }
    closedir(dir);

    if (count == 0) {
        printf("Tidak ada story di folder.\n");
        return NULL;
    }

    int pilihan;
    printf("Masukkan nomor story: ");
    scanf("%d", &pilihan); getchar();

    if (pilihan < 1 || pilihan > count) {
        printf("Pilihan tidak valid.\n");
        return NULL;
    }

    return storyNames[pilihan - 1];
}