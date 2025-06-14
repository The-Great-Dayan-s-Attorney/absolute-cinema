#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "filemanager.h"
#include "story.h"
#include "chapter.h"
#include "scene.h"
#include "stack.h"
#include "riwayat.h"

Game* createGame() {
    Game* game = malloc(sizeof(Game));
    if (game == NULL) {
        return NULL;
    }
    game->fm = createFileManager();
    game->currentStory = NULL;
    game->currentChapter = NULL;
    game->currentScene = NULL;
    game->sceneStack = createStack();
    game->storyIndex = -1;
    initRiwayat(&game->pilihanRiwayat);
    return game;
}

void startGame(Game* game) {
    if (game == NULL) {
        return;
    }
   
    int count = 0;
    StoryEntry* stories = listStories(&count);

    if (stories == NULL) {
        printf("Tidak ada cerita yang tersedia!\n");
        return;
    }
    
    printf("\n=== DAFTAR CERITA TERSEDIA ===\n");
    for (int i = 0; i < count; i++) {
        printf("%d. %s (Filename: %s)\n", i + 1, stories[i].title, stories[i].filename);
    }

    int choice;
    printf("Masukkan pilihan cerita: ");
    scanf("%d", &choice);
    getchar(); // bersihin new line
    free(stories);
    selectStory(game, choice - 1);
}

void selectStory(Game* game, int index) {
    if (game == NULL) {
        return;
    }

    int count = 0;
    StoryEntry* stories = listStories(&count);

    if (index < 0 || index >= count) {
        printf("Cerita tidak ada!\n");
        free(stories);
        return;
    } 
    
    StoryEntry nama_cerita;
    nama_cerita = stories[index];

    game->currentStory = loadStory(nama_cerita.filename);
    if (game->currentStory != NULL) {
        game->currentChapter = game->currentStory->chapters.head;
        game->currentScene = (game->currentChapter != NULL) ? game->currentChapter->firstScene : NULL;
        game->storyIndex = index;
        displayScene(game);
    }
    free(stories);
}

void loadChapter(Game* game, int chapterNum) {
    if (game == NULL || game->currentStory == NULL) {
        return;
    }

    addressChapter current = game->currentStory->chapters.head;
    int count = 1;

    while (current != NULL && count < chapterNum) {
        current = current->nextChapter;
        count++;
    }

    if (current != NULL) {
        game->currentChapter = current;
        game->currentScene = game->currentChapter->firstScene;
        displayScene(game);
    } else {
        printf("Chapter tidak ditemukan!\n");
    }
}

void selectScene(Game* game, int choiceId) {
    if (game == NULL || game->currentScene == NULL) {
        return;
    }

    for (int i = 0; i < MAX_CHOICES; i++) {
        if (game->currentScene->choices[i].id == choiceId) {
            push(game->sceneStack, game->currentScene);
            addRiwayat(&game->pilihanRiwayat, choiceId, game->currentScene->choices[i].title);
            game->currentScene = game->currentScene->choices[i].nextScene;
            if (game->currentScene == NULL) {
                loadChapter(game, 1); // pindah ke chapter berikutnya (sementara ke 1)
            } else {
                displayScene(game);
            }
            return;
        }
    }
    printf("Pilihan tidak valid!\n");
}

void displayScene(Game* game) {
    if (game == NULL || game->currentScene == NULL) {
        printf("Scene tidak tersedia!\n");
        return;
    }

    printf("\n=== SCENE: %s ===\n", game->currentScene->title);
    printf("%s\n", game->currentScene->description);

    for (int i = 0; i < MAX_CHOICES; i++) {
        if (game->currentScene->choices[i].title[0] != '\0') {
            printf("Pilihan [%d]: %s\n", game->currentScene->choices[i].id, game->currentScene->choices[i].title);
        }
    }

    printf("Masukkan 0 untuk undo, 99 untuk lihat riwayat\n");
    int choice;
    printf("Pilihan: ");
    scanf("%d", &choice);
    getchar();

    if (choice == 0) {
        undoScene(game);
    } else if (choice == 99) {
        tampilkanRiwayat(game);
        displayScene(game); // kembali ke scene
    } else {
        selectScene(game, choice);
    }
}

void undoScene(Game* game) {
    if (game == NULL || game->sceneStack == NULL) {
        printf("Tidak ada scene untuk dikembalikan!\n");
        return;
    }

    addressScene scene = pop(game->sceneStack);
    if (scene != NULL) {
        game->currentScene = scene;
        displayScene(game);
    }
}

void tampilkanRiwayat(Game* game) {
    if (game == NULL) return;
    printRiwayat(&game->pilihanRiwayat);
}

void endGame(Game* game) {
    if (game == NULL) {
        return;
    }

    if (game->fm != NULL) {
        free(game->fm);
    }
    if (game->sceneStack != NULL) {
        freeStack(game->sceneStack);
    }
    clearRiwayat(&game->pilihanRiwayat);
    free(game);
    printf("Terima kasih telah bermain!\n");
}

void saveGameState(Game* game, char* filename) {
    if (game == NULL || filename == NULL) {
        printf("Gagal menyimpan: Game atau filename tidak valid!\n");
        return;
    }

    char savePath[MAX_NAME];
    snprintf(savePath, MAX_NAME, "data/%s_save_%d.txt", filename, game->storyIndex);

    FILE* file = fopen(savePath, "w");
    if (file == NULL) {
        printf("Gagal membuat file save!\n");
        return;
    }

    fprintf(file, "StoryIndex: %d\n", game->storyIndex);
    fprintf(file, "ChapterTitle: %s\n", (game->currentChapter != NULL) ? game->currentChapter->title : "None");
    fprintf(file, "SceneTitle: %s\n", (game->currentScene != NULL) ? game->currentScene->title : "None");
    fclose(file);

    printf("Status game disimpan ke: %s\n", savePath);
}

void loadGameState(Game* game, char* filename) {
    if (game == NULL || filename == NULL) {
        printf("Gagal memuat: Game atau filename tidak valid!\n");
        return;
    }

    char savePath[MAX_NAME];
    snprintf(savePath, MAX_NAME, "data/%s_save_%d.txt", filename, game->storyIndex);

    FILE* file = fopen(savePath, "r");
    if (file == NULL) {
        printf("File save tidak ditemukan!\n");
        return;
    }

    char line[100];
    int storyIndex = -1;
    char chapterTitle[MAX_TITLE] = "";
    char sceneTitle[MAX_TITLE] = "";

    while (fgets(line, 100, file)) {
        trim(line);
        if (strncmp(line, "StoryIndex: ", 12) == 0) {
            sscanf(line + 12, "%d", &storyIndex);
        } else if (strncmp(line, "ChapterTitle: ", 14) == 0) {
            strncpy(chapterTitle, line + 14, MAX_TITLE - 1);
        } else if (strncmp(line, "SceneTitle: ", 12) == 0) {
            strncpy(sceneTitle, line + 12, MAX_TITLE - 1);
        }
    }
    fclose(file);

    if (storyIndex != -1) {
        int count = 0;
        StoryEntry* stories = listStories(&count);
        if (storyIndex >= 0 && storyIndex < count) {
            game->currentStory = loadStory(stories[storyIndex].filename);
            game->storyIndex = storyIndex;
            game->currentChapter = game->currentStory->chapters.head;
            while (game->currentChapter != NULL && strcmp(game->currentChapter->title, chapterTitle) != 0) {
                game->currentChapter = game->currentChapter->nextChapter;
            }
            game->currentScene = (game->currentChapter != NULL) ? game->currentChapter->firstScene : NULL;
            while (game->currentScene != NULL && strcmp(game->currentScene->title, sceneTitle) != 0) {
                game->currentScene = game->currentScene->nextScene;
            }
        }
        free(stories);
    }

    printf("Status game dimuat dari: %s\n", savePath);
}

void tampilkanMenuAwal(Game* game) {
    int pilihan;
    do {
        printf("\n===== MENU AWAL GAME =====\n");
        printf("1. Mulai Game Baru\n");
        printf("2. Lanjutkan dari Save File\n");
        printf("3. Keluar\n");
        printf("Masukkan pilihan (1-3): ");
        scanf("%d", &pilihan);
        getchar(); // Bersihkan newline dari buffer

        if (pilihan == 1) {
            startGame(game);
        } else if (pilihan == 2) {
            char filename[100];
            printf("Masukkan nama file save (tanpa ekstensi): ");
            scanf("%s", filename);
            getchar();
            loadGameState(game, filename);

            if (game->currentScene != NULL) {
                displayScene(game);
            } else {
                printf("Gagal melanjutkan game. Mungkin file tidak valid.\n");
            }

        } else if (pilihan == 3) {
            printf("Terima kasih! Game ditutup.\n");
        } else {
            printf("Pilihan tidak valid. Silakan coba lagi.\n");
        }
    } while (pilihan != 3);
}
