#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <direct.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>



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

//
//char* pilihStoryDariFolder(const char* basePath) {
//    DIR *dir;
//    struct dirent *entry;
//
//    static char storyNames[MAX_STORY][MAX_NAME];
//    int count = 0;
//
//    dir = opendir(basePath);
//    if (dir == NULL) {
//        perror("Tidak bisa membuka direktori");
//        return NULL;
//    }
//
//    printf("\n=== PILIH STORY DARI '%s' ===\n", basePath);
//    while ((entry = readdir(dir)) != NULL) {
//        if (entry->d_type == DT_DIR) {
//            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
//                continue;
//
//            printf("%d. %s\n", count + 1, entry->d_name);
//            strncpy(storyNames[count], entry->d_name, MAX_NAME);
//            count++;
//        }
//    }
//    closedir(dir);
//
//    if (count == 0) {
//        printf("Tidak ada story di folder.\n");
//        return NULL;
//    }
//
//    int pilihan;
//    printf("Masukkan nomor story: ");
//    scanf("%d", &pilihan); getchar();
//
//    if (pilihan < 1 || pilihan > count) {
//        printf("Pilihan tidak valid.\n");
//        return NULL;
//    }
//
//    return storyNames[pilihan - 1];
//}

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

        // Gabungkan path lengkap untuk stat()
        char fullPath[MAX_NAME * 2];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", basePath, entry->d_name);

        // Cek apakah ini folder
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
        cleanTitle[strcspn(cleanTitle, "\n")] = '\0';  // Hapus \n dari title story

        if (strcmp(cleanTitle, judulHapus) == 0) break;

        prev = curr;
        curr = curr->nextStory;
    }


    if (curr == NULL) {
        printf("Story tidak ditemukan.\n");
        return;
    }

    // Hapus dari linked list
    if (prev == NULL) {
        *listStory = curr->nextStory;
    } else {
        prev->nextStory = curr->nextStory;
    }

    // Bangun path folder
    char folderPath[200];
    snprintf(folderPath, sizeof(folderPath), "../../data/%s", judulHapus);

    // Hapus folder dan isinya
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

    free(curr); // bebaskan memori jika malloc digunakan
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

    // Simpan judul lama untuk hapus folder jika perlu
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

    // Hapus folder lama kalau judul berubah
    if (strcmp(judulLama, curr->title) != 0) {
        char path[200];
        snprintf(path, sizeof(path), "../../data/%s", judulLama);
        hapusFolder(path); // pastikan fungsi ini tersedia
    }

    // Tulis ulang ke folder baru
    writeStoryToFolder(curr);

    printf("Story berhasil diperbarui.\n");
}



void hapusFolder(const char* path) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "rmdir /s /q \"%s\"", path);
    system(cmd);
}


//void loadAllStories(addressStory* listStory, const char* folderPath) {
//    DIR *dir = opendir(folderPath);
//    if (dir == NULL) {
//        printf("Folder tidak ditemukan.\n");
//        return;
//    }
//
//    struct dirent *entry;
//    while ((entry = readdir(dir)) != NULL) {
//        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
//            char path[200];
//            snprintf(path, sizeof(path), "%s/%s/details_story.txt", folderPath, entry->d_name);
//
//            FILE *f = fopen(path, "r");
//            if (f) {
//                char line[256];
//                char title[MAX_TITLE], desc[MAX_DESCRIPTION];
//
//                fgets(line, sizeof(line), f);
//                sscanf(line, "Judul: %[^\n]", title);
//
//                fgets(line, sizeof(line), f);
//                sscanf(line, "Deskripsi: %[^\n]", desc);
//
//                fclose(f);
//
//                addressStory s = createStory(title, desc);
//                addStory(listStory, s);
//            }
//        }
//    }
//
//    closedir(dir);
//}

void loadAllStories(addressStory* listStory, const char* folderPath) {
    DIR *dir = opendir(folderPath);
    if (dir == NULL) {
        printf("Folder tidak ditemukan.\n");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip . dan ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Bangun path lengkap ke subfolder
        char folderFullPath[256];
        snprintf(folderFullPath, sizeof(folderFullPath), "%s/%s", folderPath, entry->d_name);

        // Cek apakah itu folder pakai stat
        struct stat st;
        if (stat(folderFullPath, &st) == 0 && S_ISDIR(st.st_mode)) {
            // Bangun path file detail
            char path[300];
            snprintf(path, sizeof(path), "%s/details_story.txt", folderFullPath);

            FILE *f = fopen(path, "r");
            if (f) {
                char line[256];
                char title[MAX_TITLE], desc[MAX_DESCRIPTION];

                fgets(line, sizeof(line), f);
                sscanf(line, "Judul: %[^\n]", title);

                fgets(line, sizeof(line), f);
                sscanf(line, "Deskripsi: %[^\n]", desc);

                fclose(f);

                addressStory s = createStory(title, desc);
                addStory(listStory, s);
            }
        }
    }

    closedir(dir);
}
