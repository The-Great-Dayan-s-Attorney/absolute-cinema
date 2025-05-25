#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <direct.h>


#include "story.h"
#include "chapter.h"

void deleteAllChapters(addressChapter ch) {
    addressChapter temp;
    while (ch != NULL) {
        temp = ch;
        ch = ch->nextChapter;
        free(temp);
    }
}

