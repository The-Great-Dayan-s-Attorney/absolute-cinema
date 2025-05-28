#ifndef SCENE_H
#define SCENE_H

#include "convention.h"
#include "scene.h"
#include "choices.h"
#include "chapter.h"

typedef struct Chapter* addressChapter;

typedef struct Scene* addressScene;
typedef struct Scene{
    char title[MAX_TITLE];
    char description[MAX_DESCRIPTION];

    Choice choices[MAX_CHOICES];
    addressScene nextScene;
}Scene;

addressScene createScene(const char *title, const char *desc);

void addSceneToChapter(addressChapter ch, addressScene sc);



#endif