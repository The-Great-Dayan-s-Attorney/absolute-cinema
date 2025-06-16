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
#include "game.h"
#include "filemanager.h"
#include "stack.h"
#include "riwayat.h"

#include "menuModeCreate.c"
#include "story.c"
#include "chapter.c"
#include "scene.c"
#include "queue.c"
#include "game.c"
#include "filemanager.c"
#include "stack.c"
#include "riwayat.c"


int main() {
    int pilihan;

    do {
        printf("\n=== MENU UTAMA VISUAL NOVEL ===\n");
        printf("1. Mode Build (Buat Cerita)\n");
        printf("2. Mode Play (Mainkan Cerita)\n");
        printf("3. Keluar\n");
        printf("Pilihan: ");
        scanf("%d", &pilihan);
        getchar(); // Bersihkan newline dari buffer

        switch (pilihan) {
            case 1: {
                // Mode Build
                mainMenuBuild();
                break;
            }
            case 2: {
                // Mode Play
                Game* game = createGame();
                if (game == NULL) {
                    printf("Gagal memulai game! Memori tidak cukup.\n");
                    return 1;
                }

                startGame(game);
                endGame(game); // Akhiri game setelah selesai
                break;
            }
            case 3:
                printf("Terima kasih telah menggunakan program!\n");
                break;
            default:
                printf("Pilihan tidak valid! Silakan coba lagi.\n");
        }
    } while (pilihan != 3);

    return 0;
}