#ifndef GAME_H
#define GAME_H

#include "convention.h"
#include "story.h"
#include "chapter.h"
#include "scene.h"
#include "queue.h"
#include "stack.h"
#include "riwayat.h"
#include "linkedlist.h"
#include "filemanager.h"

typedef struct Game {
    FileManager* fm;
    addressStory currentStory;
    addressChapter currentChapter;
    addressScene currentScene;
    Stack sceneStack;
    int storyIndex;
    RiwayatList pilihanRiwayat;
} Game;

Game* createGame();
int startGame(Game* game);
void selectStory(Game* game);
void selectScene(Game* game);
void displayScene(Game* game);
void undoScene(Game* game);
void tampilkanRiwayat(Game* game);
void endGame(Game* game);
void saveGameState(Game* game);
void loadGameState(Game* game);
int tampilkanMenuAwal(Game* game);
int playerMode(); // Diperbarui dari void menjadi int
int validatePlayerInput(int input, int maxChoices);
void showHowToPlay();
void tampilkanHeaderVisualNovel();
#endif