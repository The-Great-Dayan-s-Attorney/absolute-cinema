#ifndef GAME_H
#define GAME_H

#include "filemanager.h"
#include "story.h"
#include "chapter.h"
#include "scene.h"
#include "queue.h"
#include "stack.h"
#include "riwayat.h"

#include "convention.h"

typedef struct {
    FileManager* fm;
    addressStory currentStory;
    addressChapter currentChapter;
    addressScene currentScene;
    Stack* sceneStack; // Untuk menyimpan riwayat scene (undo)
    int storyIndex;
    RiwayatList pilihanRiwayat;
} Game;

Game* createGame();
void startGame(Game* game);
void selectStory(Game* game, int index);
void loadChapter(Game* game, int chapterNum);
void selectScene(Game* game, int choiceId);
void displayScene(Game* game);
void undoScene(Game* game);
void endGame(Game* game);
void saveGameState(Game* game, char* filename);
void loadGameState(Game* game, char* filename);
void tampilkanRiwayat(Game* game);
void tampilkanMenuAwal(Game* game);

#endif