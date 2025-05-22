#ifndef CHOICES_H
#define CHOICES_H

#include <stdio.h>
#include <stdlib.h>

#include "convention.h"
#include "scene.h"

typedef struct Choices* choice;
typedef struct Choices{
    int id;
    char title[MAX_TITLE];

    choice nextChoice;
    scene nextScene;
}Choices;

#endif