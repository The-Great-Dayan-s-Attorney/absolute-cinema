#include <stdio.h>
#include <stdlib.h>

#include "game.h"

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
    return game;
}

void startGame (Game* game) {
    if (game == NULL) {
        return 1;
    }
   
    int count = 0;
    StoryEntry* stories = listStories(&count);

    if (stories == NULL) {
        printf("Tidak ada cerita yang tersedia!");
        return 1;
    }
    
    printf("\n=== DAFTAR CERITA TERSEDIA ===\n");
    for (int i = 0; i < count; i++) {
    printf("%d. %s (Filename: %s)\n", i + 1, stories[i].title, stories[i].filename);
    }

    int choice;
    printf("Masukkan pilihan cerita : ");
    scanf("%d", choice);
    selectStory(game, choice - 1); 
}

void selectStory (Game* game, int index) {
    int count;
    StoryEntry* stories = listStories(&count);

    if (game == NULL) {
        return 1;
    }

    if (index > count || index < count) {
        printf("Cerita tidak ada!");
        return 1;
    } 

    // strcpy()
    
}