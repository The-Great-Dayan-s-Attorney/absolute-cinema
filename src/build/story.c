#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <direct.h>
#include <errno.h>
#include <dirent.h>

#include "story.h"
#include "chapter.h"
#include "convention.h"
#include "queue.h"
#include "filemanager.h"

addressStory createStory(char *title, char *description) {
    addressStory S = (addressStory)malloc(sizeof(Story));
    if (S == NULL) {
        printf("gagal\n");
        return NULL;
    }

    strcpy(S->title, title);
    strcpy(S->description, description);
    S->nextStory = NULL;
    initQueue(&S->chapters);
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

    deleteAllChapters(&current->chapters);

    if (prev == NULL) {
        *headStory = current->nextStory;
    } else {
        prev->nextStory = current->nextStory;
    }

    free(current);
    printf("Story dengan judul \"%s\" berhasil dihapus.\n", titleStory);
}

void printAllStory(addressStory s) {
    int index = 1;
    while (s != NULL) {
        printf("Story %d:\n", index++);
        printf("  Judul     : %s\n", s->title);
        printf("  Deskripsi : %s\n", s->description);
        s = s->nextStory;
    }
    printf("NULL\n");
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
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullPath[MAX_NAME * 2];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", basePath, entry->d_name);

        struct stat st;
        if (stat(fullPath, &st) == 0 && S_ISDIR(st.st_mode)) {
            printf("%d. %s\n", count + 1, entry->d_name);
            strncpy(storyNames[count], entry->d_name, MAX_NAME - 1);
            storyNames[count][MAX_NAME - 1] = '\0';
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

void hapusStory(addressStory *listStory) {
    if (*listStory == NULL) {
        printf("Belum ada story yang bisa dihapus.\n");
        return;
    }

    printAllStory(*listStory);

    char judulHapus[100];
    printf("Masukkan judul story yang ingin dihapus: ");
    fgets(judulHapus, 100, stdin);
    judulHapus[strcspn(judulHapus, "\n")] = '\0';

    addressStory prev = NULL, curr = *listStory;
    while (curr != NULL) {
        char cleanTitle[MAX_TITLE];
        strncpy(cleanTitle, curr->title, MAX_TITLE);
        cleanTitle[strcspn(cleanTitle, "\n")] = '\0';

        if (strcmp(cleanTitle, judulHapus) == 0) break;

        prev = curr;
        curr = curr->nextStory;
    }

    if (curr == NULL) {
        printf("Story tidak ditemukan.\n");
        return;
    }

    if (prev == NULL) {
        *listStory = curr->nextStory;
    } else {
        prev->nextStory = curr->nextStory;
    }

    char folderPath[200];
    snprintf(folderPath, sizeof(folderPath), "../../data/%s", judulHapus);

    #ifdef _WIN32
        char command[256];
        snprintf(command, sizeof(command), "rmdir /s /q \"%s\"", folderPath);
    #else
        char command[256];
        snprintf(command, sizeof(command), "rm -rf \"%s\"", folderPath);
    #endif

    int result = system(command);
    if (result == 0) {
        printf("Story \"%s\" berhasil dihapus beserta foldernya.\n", judulHapus);
    } else {
        printf("Gagal menghapus folder \"%s\".\n", judulHapus);
    }

    free(curr);
}

void editStory(addressStory listStory) {
    printAllStory(listStory);

    int index;
    printf("Masukkan nomor story yang ingin diedit: ");
    scanf("%d", &index); getchar();

    addressStory curr = listStory;
    int i = 1;
    while (curr && i < index) {
        curr = curr->nextStory;
        i++;
    }

    if (curr == NULL) {
        printf("Story tidak ditemukan.\n");
        return;
    }

    char judulLama[MAX_TITLE];
    strncpy(judulLama, curr->title, MAX_TITLE);

    printf("Judul lama: %s", curr->title);
    printf("Deskripsi lama: %s", curr->description);

    printf("Masukkan judul baru (ENTER jika tidak diubah): ");
    char newTitle[MAX_TITLE];
    fgets(newTitle, MAX_TITLE, stdin);
    if (newTitle[0] != '\n') {
        newTitle[strcspn(newTitle, "\n")] = '\0';
        strncpy(curr->title, newTitle, MAX_TITLE);
    }

    printf("Masukkan deskripsi baru (ENTER jika tidak diubah): ");
    char newDesc[MAX_DESCRIPTION];
    fgets(newDesc, MAX_DESCRIPTION, stdin);
    if (newDesc[0] != '\n') {
        newDesc[strcspn(newDesc, "\n")] = '\0';
        strncpy(curr->description, newDesc, MAX_DESCRIPTION);
    }

    if (strcmp(judulLama, curr->title) != 0) {
        char path[200];
        snprintf(path, sizeof(path), "../../data/%s", judulLama);
        hapusFolder(path);
    }

    fm_save_story(curr);

    printf("Story berhasil diperbarui.\n");
}

void hapusFolder(const char* path) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "rmdir /s /q \"%s\"", path);
    system(cmd);
}

void pilihDanBukaStory(addressStory *listStory) {
    char* storyName = pilihStoryDariFolder("../../data");

    if (storyName != NULL) {
        char storyPath[200];
        snprintf(storyPath, sizeof(storyPath), "../../data/%s/details_story.txt", storyName);

        FILE *f = fopen(storyPath, "r");
        if (f == NULL) {
            printf("Gagal membuka file: %s\n", storyPath);
            return;
        }

        char line[256];
        char title[100], desc[200];

        fgets(line, sizeof(line), f);
        sscanf(line, "Judul: %[^\n]", title);

        fgets(line, sizeof(line), f);
        sscanf(line, "Deskripsi: %[^\n]", desc);

        fclose(f);

        addressStory s = createStory(title, desc);
        addStory(listStory, s);

        menuStory(s);
    } else {
        printf("Tidak ada story yang dipilih.\n");
    }
}

void tambahChapter(addressStory s) {
    char title[MAX_TITLE], desc[MAX_DESCRIPTION];
    printf("Judul Chapter: "); fgets(title, MAX_TITLE, stdin);
    printf("Deskripsi Chapter: "); fgets(desc, MAX_DESCRIPTION, stdin);

    addressChapter ch = createChapter(title, desc);
    enqueue(&s->chapters, ch);

    int index = getChapterCount(&s->chapters);
    fm_save_chapter_with_scenes(s, ch, index);
}

void pilihChapterDariStory(addressStory s) {
    addressChapter curr = s->chapters.head;
    if (curr == NULL) {
        printf("Belum ada chapter.\n");
        return;
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
        return;
    }

    addressChapter ch = fm_load_chapter(s, pilihChapter);
    if (ch != NULL) {
        menuChapter(s, ch, pilihChapter);
    } else {
        printf("Gagal memuat chapter.\n");
    }
}

void editChapterDariStory(addressStory s) {
    if (isQueueEmpty(s->chapters)) {
        printf("Belum ada chapter untuk diedit.\n");
        return;
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
        return;
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
}
