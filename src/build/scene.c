#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "scene.h"
#include "chapter.h"
#include "choices.h"


addressScene createScene(const char *title, const char *desc) {
    addressScene sc = (addressScene)malloc(sizeof(Scene));
    if (sc != NULL) {
        strncpy(sc->title, title, MAX_TITLE - 1);
        sc->title[MAX_TITLE - 1] = '\0';

        strncpy(sc->description, desc, MAX_DESCRIPTION - 1);
        sc->description[MAX_DESCRIPTION - 1] = '\0';

        for (int i = 0; i < MAX_CHOICES; i++) {
            sc->choices[i].id = -1;
            sc->choices[i].title[0] = '\0';
            sc->choices[i].nextScene = NULL;
        }
        sc->nextScene = NULL;
    }
    return sc;
}

void addSceneToChapter(addressChapter ch, addressScene sc) {
    if (ch->firstScene == NULL) {
        ch->firstScene = sc;
    } else {
        addressScene temp = ch->firstScene;
        while (temp->nextScene != NULL) {
            temp = temp->nextScene;
        }
        temp->nextScene = sc;
    }
}
