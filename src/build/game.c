#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "filemanager.h"

Game* createGame() {
    Game* game = (Game*)malloc(sizeof(Game));
    if (game == NULL) return NULL;

    game->fm = createFileManager();
    game->currentStory = NULL;
    game->currentChapter = NULL;
    game->currentScene = NULL;
    createStack(&game->sceneStack);
    game->storyIndex = -1;
    initRiwayat(&game->pilihanRiwayat);

    return game;
}

void startGame(Game* game) {
    tampilkanMenuAwal(game);
}

void selectStory(Game* game) {
    int count;
    StoryEntry* stories = listStories(&count);
    if (count == 0) {
        printf("Tidak ada cerita yang tersedia.\n");
        return;
    }

    printf("\n=== PILIH CERITA ===\n");
    for (int i = 0; i < count; i++) {
        printf("%d. %s\n", i + 1, stories[i].title);
    }

    int choice;
    printf("Masukkan nomor cerita: ");
    scanf("%d", &choice); getchar();
    if (choice < 1 || choice > count) {
        printf("Pilihan tidak valid.\n");
        free(stories);
        return;
    }

    game->storyIndex = choice - 1;
    game->currentStory = fm_load_story(stories[game->storyIndex].filename);
    free(stories);

    if (game->currentStory == NULL) {
        printf("Gagal memuat cerita.\n");
        return;
    }

    selectScene(game);
}

void selectScene(Game* game) {
    if (game->currentStory == NULL) {
        printf("Tidak ada cerita yang dipilih.\n");
        return;
    }

    if (game->currentChapter == NULL) {
        game->currentChapter = game->currentStory->chapters.head;
        if (game->currentChapter == NULL) {
            printf("Tidak ada chapter dalam cerita ini.\n");
            return;
        }
    }

    if (game->currentScene == NULL) {
        game->currentScene = game->currentChapter->firstScene;
        if (game->currentScene == NULL) {
            printf("Tidak ada scene dalam chapter ini.\n");
            return;
        }
        push(&game->sceneStack, game->currentScene);
    }

    displayScene(game);
}

void displayScene(Game* game) {
    if (game->currentScene == NULL) {
        printf("Tidak ada scene untuk ditampilkan.\n");
        return;
    }

    printf("\n=== SCENE: %s ===\n", game->currentScene->title);
    printf("%s\n", game->currentScene->description);

    int choiceCount = 0;
    for (int i = 0; i < MAX_CHOICES; i++) {
        if (game->currentScene->choices[i].id != -1 && game->currentScene->choices[i].title[0] != '\0') {
            printf("%d. %s\n", i + 1, game->currentScene->choices[i].title);
            choiceCount++;
        }
    }

    if (choiceCount == 0) {
        printf("Tidak ada pilihan. Cerita selesai.\n");
        endGame(game);
        return;
    }

    int input;
    do {
        input = validatePlayerInput(playerMode(game), choiceCount);
        if (input == 0) {
            printf("Input tidak valid. Silakan pilih angka dari 1 sampai %d, atau 0 untuk undo, -1 untuk menu.\n", choiceCount);
        }
    } while (input == 0);

    if (input == -1) { // Input 0 untuk undo
        undoScene(game);
    } else if (input == -2) { // Input -1 untuk kembali ke menu awal
        tampilkanMenuAwal(game);
    } else if (input >= 1 && input <= choiceCount) {
        addRiwayat(&game->pilihanRiwayat, input, game->currentScene->choices[input - 1].title);
        game->currentScene = game->currentScene->choices[input - 1].nextScene;
        push(&game->sceneStack, game->currentScene);
        displayScene(game);
    }
}

void undoScene(Game* game) {
    if (isEmptyStack(&game->sceneStack) || game->sceneStack.top == NULL || game->sceneStack.top->next == NULL) {
        printf("Tidak ada scene sebelumnya untuk diundo.\n");
        displayScene(game); // Tetap tampilkan scene saat ini
        return;
    }

    pop(&game->sceneStack); // Hapus scene saat ini
    if (game->sceneStack.top != NULL) {
        game->currentScene = game->sceneStack.top->scene;
    } else {
        game->currentScene = NULL;
    }
    displayScene(game);
}

void tampilkanRiwayat(Game* game) {
    printRiwayat(&game->pilihanRiwayat);
}

void endGame(Game* game) {
    printf("Terima kasih telah bermain!\n");
    freeStack(&game->sceneStack);
    clearRiwayat(&game->pilihanRiwayat);
    free(game);
    game = createGame(); // Buat ulang game untuk reset state
    tampilkanMenuAwal(game); // Kembali ke menu awal
}

void saveGameState(Game* game) {
    if (game->currentStory == NULL || game->currentChapter == NULL || game->currentScene == NULL) {
        printf("Tidak ada cerita aktif untuk disimpan.\n");
        return;
    }

    char filename[50];
    printf("Masukkan nama file untuk menyimpan (tanpa ekstensi): ");
    fgets(filename, 50, stdin);
    filename[strcspn(filename, "\n")] = '\0';

    char filepath[100];
    snprintf(filepath, 100, "saves/%s.txt", filename);

    FILE* file = fopen(filepath, "w");
    if (file == NULL) {
        printf("Gagal membuka file untuk menyimpan.\n");
        return;
    }

    fprintf(file, "Story: %s\n", game->currentStory->title);
    fprintf(file, "Chapter: %s\n", game->currentChapter->title);
    fprintf(file, "Scene: %s\n", game->currentScene->title);
    fclose(file);

    printf("Game state disimpan ke %s.\n", filepath);
}

void loadGameState(Game* game) {
    char filename[50];
    printf("Masukkan nama file untuk memuat (tanpa ekstensi): ");
    fgets(filename, 50, stdin);
    filename[strcspn(filename, "\n")] = '\0';

    char filepath[100];
    snprintf(filepath, 100, "saves/%s.txt", filename);

    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        printf("File tidak ditemukan.\n");
        return;
    }

    freeStack(&game->sceneStack);
    clearRiwayat(&game->pilihanRiwayat);
    game->currentStory = NULL;
    game->currentChapter = NULL;
    game->currentScene = NULL;

    char line[256];
    int storyIndex = -1;
    while (fgets(line, 256, file)) {
        line[strcspn(line, "\n")] = '\0';
        if (strncmp(line, "Story: ", 7) == 0) {
            char storyTitle[100];
            strncpy(storyTitle, line + 7, 100);
            int count;
            StoryEntry* stories = listStories(&count);
            for (int i = 0; i < count; i++) {
                if (strcmp(stories[i].title, storyTitle) == 0) {
                    storyIndex = i;
                    break;
                }
            }
            free(stories);
            if (storyIndex != -1) {
                game->currentStory = fm_load_story(stories[storyIndex].filename);
            }
        } else if (strncmp(line, "Chapter: ", 9) == 0) {
            char chapterTitle[100];
            strncpy(chapterTitle, line + 9, 100);
            addressChapter curr = game->currentStory->chapters.head;
            while (curr != NULL) {
                if (strcmp(curr->title, chapterTitle) == 0) {
                    game->currentChapter = curr;
                    break;
                }
                curr = curr->nextChapter;
            }
        } else if (strncmp(line, "Scene: ", 7) == 0) {
            char sceneTitle[100];
            strncpy(sceneTitle, line + 7, 100);
            addressScene curr = game->currentChapter->firstScene;
            while (curr != NULL) {
                if (strcmp(curr->title, sceneTitle) == 0) {
                    game->currentScene = curr;
                    break;
                }
                curr = curr->nextScene;
            }
        }
    }
    fclose(file);

    if (game->currentStory != NULL && game->currentChapter != NULL && game->currentScene != NULL) {
        printf("Game state dimuat dari %s.\n", filepath);
        push(&game->sceneStack, game->currentScene);
        selectScene(game);
    } else {
        printf("Gagal memuat game state.\n");
    }
}

void tampilkanMenuAwal(Game* game) {
    int pilihan;
    do {
        printf("\n=== MENU AWAL ===\n");
        printf("1. Mulai Permainan Baru\n");
        printf("2. Lanjutkan Permainan\n");
        printf("3. Simpan Permainan\n");
        printf("4. Muat Permainan\n");
        printf("5. Lihat Riwayat Pilihan\n");
        printf("6. Keluar\n");
        printf("Pilihan: ");
        scanf("%d", &pilihan); getchar();

        switch (pilihan) {
            case 1:
                selectStory(game);
                break;
            case 2:
                if (game->currentStory != NULL && game->currentChapter != NULL && game->currentScene != NULL) {
                    selectScene(game);
                } else {
                    printf("Tidak ada permainan yang sedang berlangsung.\n");
                }
                break;
            case 3:
                saveGameState(game);
                break;
            case 4:
                loadGameState(game);
                break;
            case 5:
                tampilkanRiwayat(game);
                break;
            case 6:
                printf("Keluar dari permainan.\n");
                freeStack(&game->sceneStack);
                clearRiwayat(&game->pilihanRiwayat);
                free(game);
                break;
            default:
                printf("Pilihan tidak valid. Silakan coba lagi.\n");
        }
    } while (pilihan != 6);
}

int playerMode(Game* game) {
    int input;
    printf("Masukkan pilihan (1-%d), 0 untuk Undo, atau -1 untuk kembali ke Menu Awal: ", MAX_CHOICES);
    scanf("%d", &input); getchar();
    return input;
}

int validatePlayerInput(int input, int maxChoices) {
    if (input == 0) return -1; // Untuk undo
    if (input == -1) return -2; // Untuk kembali ke menu awal
    if (input >= 1 && input <= maxChoices) return input;
    return 0; // Input tidak valid, ulangi
}
