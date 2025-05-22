#ifndef SCENE_H
#define SCENE_H

#include <stdio.h>
#include <stdlib.h>

#include "convention.h"

typedef struct Scene* scene;
typedef struct Scene{
    int id;
    char description[MAX_DESCRIPTION];

    
}Scene;


#endif