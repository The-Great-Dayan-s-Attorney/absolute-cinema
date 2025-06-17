#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif
#include "game.h"
#include "filemanager.h"

Game* createGame() {
    Game* game = (Game*)malloc(sizeof(Game));
    if (game == NULL) return NULL;

    game->fm = createFileManager();
    Stack* stack = createStack();
    if (game->fm == NULL || stack == NULL) {
        free(game->fm);
        free(stack);
        free(game);
        return NULL;
    }
    game->sceneStack = *stack;
    free(stack);
    game->currentStory = NULL;
    game->currentChapter = NULL;
    game->currentScene = NULL;
    game->storyIndex = -1;
    initRiwayat(&game->pilihanRiwayat);

    return game;
}

int startGame(Game* game) {
    return tampilkanMenuAwal(game);
}

void selectStory(Game* game) {
    int count;
    StoryEntry* stories = listStories(&count);
    if (count == 0 || stories == NULL) {
        printf("Tidak ada cerita yang tersedia.\n");
        free(stories);
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

int countChapters(addressStory story) {
    int count = 0;
    addressChapter curr = story->chapters.head;
    while (curr != NULL) {
        count++;
        curr = curr->nextChapter;
    }
    return count;
}

void selectScene(Game* game) {
    if (game == NULL || game->currentStory == NULL) {
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
    if (game == NULL || game->currentScene == NULL) {
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
        addressChapter nextChapter = game->currentChapter->nextChapter;
        if (nextChapter != NULL) {
            game->currentChapter = nextChapter;
            game->currentScene = nextChapter->firstScene;
            if (game->currentScene == NULL) {
                printf("Tidak ada scene dalam chapter berikutnya.\n");
                endGame(game);
                return;
            }
            freeStack(&game->sceneStack);
            Stack* stack = createStack();
            if (stack != NULL) {
                game->sceneStack = *stack;
                free(stack);
            }
            push(&game->sceneStack, game->currentScene);
            displayScene(game);
        } else {
            printf("Tidak ada chapter berikutnya. Cerita selesai.\n");
            endGame(game);
        }
        return;
    }

    int input;
    do {
        input = validatePlayerInput(playerMode(game), choiceCount);
        if (input == 0) {
            printf("Input tidak valid. Silakan pilih angka dari 1 sampai %d, atau 0 untuk undo, -1 untuk menu.\n", choiceCount);
        }
    } while (input == 0);

    if (input == -1) {
        undoScene(game);
    } else if (input == -2) {
        tampilkanMenuAwal(game);
    } else if (input >= 1 && input <= choiceCount) {
        addRiwayat(&game->pilihanRiwayat, input, game->currentScene->choices[input - 1].title);
        game->currentScene = game->currentScene->choices[input - 1].nextScene;
        push(&game->sceneStack, game->currentScene);
        displayScene(game);
    }
}

void undoScene(Game* game) {
    if (game == NULL) {
        printf("Game tidak diinisialisasi.\n");
        return;
    }

    if (isEmptyStack(&game->sceneStack)) {
        printf("Tidak ada scene sebelumnya untuk diundo.\n");
        if (game->currentScene != NULL) {
            displayScene(game);
        } else {
            printf("Scene saat ini tidak valid.\n");
        }
        return;
    }

    if (game->sceneStack.top == NULL || game->sceneStack.top->next == NULL) {
        printf("Debug: Stack has only one scene or top is NULL\n");
        printf("Tidak ada scene sebelumnya untuk diundo.\n");
        if (game->currentScene != NULL) {
            displayScene(game);
        } else {
            printf("Scene saat ini tidak valid.\n");
        }
        return;
    }

    pop(&game->sceneStack);
    game->currentScene = game->sceneStack.top != NULL ? game->sceneStack.top->scene : NULL;
    if (game->currentScene != NULL) {
        displayScene(game);
    } else {
        printf("Tidak ada scene untuk ditampilkan setelah undo.\n");
    }
}

void tampilkanRiwayat(Game* game) {
    printRiwayat(&game->pilihanRiwayat);
}

void endGame(Game* game) {
    if (game == NULL) return;

    printf("Terima kasih telah bermain!\n");
    freeStack(&game->sceneStack);
    clearRiwayat(&game->pilihanRiwayat);
    game->currentStory = NULL;
    game->currentChapter = NULL;
    game->currentScene = NULL;
    game->storyIndex = -1;
    Stack* stack = createStack();
    if (stack != NULL) {
        game->sceneStack = *stack;
        free(stack);
    }
    initRiwayat(&game->pilihanRiwayat);
}

void saveGameState(Game* game) {
    if (game == NULL || game->currentStory == NULL || game->currentChapter == NULL || game->currentScene == NULL) {
        printf("Tidak ada cerita aktif untuk disimpan.\n");
        return;
    }

#ifdef _WIN32
    _mkdir("../../saves");
#else
    mkdir("../../saves", 0755);
#endif

    char filename[50];
    printf("Masukkan nama file untuk menyimpan (tanpa ekstensi): ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';

    char filepath[100];
    snprintf(filepath, sizeof(filepath), "../../saves/%s.txt", filename);

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
    if (game == NULL) {
        printf("Game tidak diinisialisasi.\n");
        return;
    }

    char filename[50];
    printf("Masukkan nama file untuk memuat (tanpa ekstensi): ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';

    char filepath[100];
    snprintf(filepath, sizeof(filepath), "../../saves/%s.txt", filename);

    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        return;
    }

    freeStack(&game->sceneStack);
    clearRiwayat(&game->pilihanRiwayat);
    game->currentStory = NULL;
    game->currentChapter = NULL;
    game->currentScene = NULL;

    char line[256];
    int storyIndex = -1;
    StoryEntry* stories = NULL;
    char storyFilename[100] = "";
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        if (strncmp(line, "Story: ", 7) == 0) {
            char storyTitle[100];
            strncpy(storyTitle, line + 7, sizeof(storyTitle));
            storyTitle[sizeof(storyTitle) - 1] = '\0';
            int count;
            stories = listStories(&count);
            if (stories == NULL) {
                printf("Gagal memuat daftar cerita.\n");
                fclose(file);
                return;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(stories[i].title, storyTitle) == 0) {
                    storyIndex = i;
                    strncpy(storyFilename, stories[i].filename, sizeof(storyFilename));
                    storyFilename[sizeof(storyFilename) - 1] = '\0';
                    break;
                }
            }
            free(stories);
            stories = NULL;
            if (storyIndex != -1) {
                game->currentStory = fm_load_story(storyFilename);
                if (game->currentStory == NULL) {
                    printf("Gagal memuat cerita: %s\n", storyTitle);
                    fclose(file);
                    return;
                }
            } else {
                printf("Cerita tidak ditemukan: %s\n", storyTitle);
                fclose(file);
                return;
            }
        } else if (strncmp(line, "Chapter: ", 9) == 0 && game->currentStory != NULL) {
            char chapterTitle[100];
            strncpy(chapterTitle, line + 9, sizeof(chapterTitle));
            chapterTitle[sizeof(chapterTitle) - 1] = '\0';
            addressChapter curr = game->currentStory->chapters.head;
            while (curr != NULL) {
                if (strcmp(curr->title, chapterTitle) == 0) {
                    game->currentChapter = curr;
                    break;
                }
                curr = curr->nextChapter;
            }
            if (game->currentChapter == NULL) {
                printf("Chapter tidak ditemukan: %s\n", chapterTitle);
                fclose(file);
                return;
            }
        } else if (strncmp(line, "Scene: ", 7) == 0 && game->currentChapter != NULL) {
            char sceneTitle[100];
            strncpy(sceneTitle, line + 7, sizeof(sceneTitle));
            sceneTitle[sizeof(sceneTitle) - 1] = '\0';
            addressScene curr = game->currentChapter->firstScene;
            while (curr != NULL) {
                if (strcmp(curr->title, sceneTitle) == 0) {
                    game->currentScene = curr;
                    break;
                }
                curr = curr->nextScene;
            }
            if (game->currentScene == NULL) {
                printf("Scene tidak ditemukan: %s\n", sceneTitle);
                fclose(file);
                return;
            }
        }
    }
    fclose(file);

    if (game->currentStory != NULL && game->currentChapter != NULL && game->currentScene != NULL) {
        push(&game->sceneStack, game->currentScene);
        printf("Game state dimuat dari %s.\n", filepath);
        selectScene(game);
    } else {
        printf("Gagal memuat game state: data tidak lengkap.\n");
        if (game->currentStory == NULL) printf("Cerita tidak dimuat.\n");
        if (game->currentChapter == NULL) printf("Chapter tidak dimuat.\n");
        if (game->currentScene == NULL) printf("Scene tidak dimuat.\n");
    }
}

int tampilkanMenuAwal(Game* game) {
    int pilihan;
    do {
        printf("\n=== MENU AWAL ===\n");
        printf("1. Mulai Permainan Baru\n");
        printf("2. Lanjutkan Permainan\n");
        printf("3. Simpan Permainan\n");
        printf("4. Muat Permainan\n");
        printf("5. Lihat Riwayat Pilihan\n");
        printf("6. Kembali ke Menu Utama\n");
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
                printf("Kembali ke menu utama.\n");
                endGame(game);
                return 1;
            default:
                printf("Pilihan tidak valid. Silakan coba lagi.\n");
        }
    } while (pilihan != 6);
    return 0;
}

int playerMode(Game* game) {
    int input;
    printf("Masukkan pilihan (1-%d), 0 untuk Undo, atau -1 untuk kembali ke Menu Awal: ", MAX_CHOICES);
    if (scanf("%d", &input) != 1) {
        while (getchar() != '\n');
        return 0;
    }
    getchar();
    return input;
}

int validatePlayerInput(int input, int maxChoices) {
    if (input == 0) return -1;
    if (input == -1) return -2;
    if (input >= 1 && input <= maxChoices) return input;
    return 0;
}

void showHowToPlay() {
    printf("\n=== CARA BERMAIN ===\n");
    printf("Game ini adalah visual novel interaktif.\n\n");

    printf("MODE BUILD (Buat Cerita):\n");
    printf(" - Tambah story dan chapter.\n");
    printf(" - Tiap chapter berisi scene (adegan) dan pilihan.\n");
    printf(" - Scene dapat bercabang ke scene lain berdasarkan pilihan.\n\n");

    printf("MODE PLAY (Mainkan Cerita):\n");
    printf(" - Pilih cerita dan mainkan dari awal.\n");
    printf(" - Buat pilihan yang akan membawa ke jalur cerita berbeda.\n");
    printf(" - Bisa simpan dan lanjutkan permainan.\n\n");

    printf("Tips:\n");
    printf(" - Gunakan deskripsi dan judul yang menarik.\n");
    printf(" - Rancang alur cerita dengan logis dan variatif.\n");
    printf(" - Simpan perubahan secara berkala agar tidak hilang.\n");

    printf("\nTekan ENTER untuk kembali...\n");
    getchar();
}

void tampilkanHeaderVisualNovel() {
    printf("\n");

    printf("+====================================+\n");
    printf("|       VISUAL NOVEL CLI EDITION     |\n");
    printf("|------------------------------------|\n");
    printf("|  \"In the shadows of choices, lies  |\n");
    printf("|     a truth yet untold...\"         |\n");
    printf("+====================================+\n");
    printf("\n");
}
