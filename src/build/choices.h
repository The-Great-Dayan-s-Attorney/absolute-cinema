#ifndef CHOICES_H
#define CHOICES_H

#include <stdio.h>
#include <stdlib.h>

#include "convention.h"
#include "scene.h"

typedef struct Scene* addressScene;

typedef struct Choice {
    int id;
    char title[MAX_TITLE];
    addressScene nextScene;  // Harus pointer ke Scene
} Choice;


#endif