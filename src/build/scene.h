#ifndef SCENE_H
#define SCENE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "convention.h"
#include "choices.h"
#include "chapter.h"
#include "story.h"

typedef struct Chapter* addressChapter;

typedef struct Scene* addressScene;
typedef struct Scene {
    int id;
    char title[MAX_TITLE];
    char description[MAX_DESCRIPTION];

    Choice choices[MAX_CHOICES];
    int choiceCount;
    addressScene nextScene;
} Scene;

addressScene createScene(const char *title, const char *desc, int id);

void addSceneToChapter(addressChapter ch, addressScene sc);

addressScene findSceneByID(addressChapter ch, int id);

void printSceneStructure(addressChapter ch);

void printAllScenes(addressChapter ch);

bool hapusSceneDariChapter(addressChapter ch, int sceneID);

#endif