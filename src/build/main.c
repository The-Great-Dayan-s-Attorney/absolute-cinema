#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "story.h"
#include "chapter.h"
#include "queue.h"
#include "scene.h"
#include "choices.h"

#include "story.c"
#include "chapter.c"
#include "queue.c"
#include "scene.c"




int main(){
    addressStory ListStory = NULL;

    addStory(&ListStory , createStory("Cerita 1", "deskripsi deskripsian"));
    addStory(&ListStory, createStory("Cerita 2", "gitulah"));

    printAllStory(ListStory); 

    addressStory current = ListStory;
    while (current != NULL) {
        writeStoryToFolder(current); 
        current = current->nextStory;
    }

    deleteStoryByTitle(&ListStory, "Cerita 1");

    printAllStory(ListStory);

    Queue S1;

    createQueue(&S1);

    enqueue(&S1, createChapter("Chapter 1", "Deskripsi Chapter 1 lah ya"));

    enqueue(&S1, createChapter("Chapter 2", "Deskripsi Chapter 2 lah ya"));

    enqueue(&S1, createChapter("Chapter 3", "Deskripsi Chapter 3 lah ya"));

    printQueue(&S1);


    addressChapter ch1 = createChapter("Bab 1", "Awal petualanganmu dimulai.");

    addressScene sc1 = createScene("Hutan Gelap", "Kamu terbangun di tengah hutan yang gelap.");
    addressScene sc2 = createScene("Gua Tersembunyi", "Kamu melihat gua tersembunyi di balik semak.");
    addressScene sc3 = createScene("Danau Misterius", "Sebuah danau berkilau di bawah cahaya bulan.");

    // Hubungkan scenes ke chapter
    addSceneToChapter(ch1, sc1);
    addSceneToChapter(ch1, sc2);
    addSceneToChapter(ch1, sc3);

    // Tambahkan pilihan di sc1
    sc1->choices[0].id = 1;
    strncpy(sc1->choices[0].title, "Masuk ke gua", MAX_TITLE);
    sc1->choices[0].nextScene = sc2;

    sc1->choices[1].id = 2;
    strncpy(sc1->choices[1].title, "Pergi ke danau", MAX_TITLE);
    sc1->choices[1].nextScene = sc3;

    // Tambahkan pilihan di sc2
    sc2->choices[0].id = 1;
    strncpy(sc2->choices[0].title, "Kembali ke hutan", MAX_TITLE);
    sc2->choices[0].nextScene = sc1;

    // Tampilkan semua isi chapter
    printChapter(ch1);

    return 0;
}