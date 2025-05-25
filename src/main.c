#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "story.h"
#include "chapter.h"
#include "story.c"

#include "chapter.c"




int main(){
    addressStory ListStory = NULL;

    addStory(&ListStory , createStory("Cerita 1", "deskripsi deskripsian"));
    addStory(&ListStory, createStory("Cerita 2", "gitulah"));

    printAllStory(ListStory); 

    addressStory current = ListStory;
    while (current != NULL) {
        writeStoryToFolder(current);  // Simpan satu per satu ke foldernya
        current = current->nextStory;
    }

    deleteStoryByTitle(&ListStory, "Cerita 1");

    printAllStory(ListStory);

    return 0;
}