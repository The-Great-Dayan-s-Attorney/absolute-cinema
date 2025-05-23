#include <stdio.h>
#include <stdlib.h>
#include "story.h"
#include "story.c"


int main(){
    story ListStory = NULL;

    addStory(&ListStory , createStory("Cerita 1", "deskripsi deskripsian"));
    addStory(&ListStory, createStory("Cerita 2", "gitulah"));

    printAllStory(ListStory); 

    story current = ListStory;
    while (current != NULL) {
        writeStoryToFolder(current);  // Simpan satu per satu ke foldernya
        current = current->nextStory;
    }
    return 0;
}