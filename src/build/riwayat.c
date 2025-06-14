#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "riwayat.h"

void initRiwayat(RiwayatList* list) {
    list->head = NULL;
    list->tail = NULL;
}

void addRiwayat(RiwayatList* list, int id, const char* pilihan) {
    Riwayat* newNode = (Riwayat*)malloc(sizeof(Riwayat));
    if (newNode == NULL) {
        printf("Gagal mengalokasikan memori untuk riwayat.\n");
        return;
    }
    newNode->id = id;
    strncpy(newNode->pilihan, pilihan, MAX_TITLE - 1);
    newNode->pilihan[MAX_TITLE - 1] = '\0';
    newNode->next = NULL;

    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
}

void printRiwayat(RiwayatList* list) {
    printf("\n=== RIWAYAT PILIHAN ===\n");
    Riwayat* current = list->head;
    int i = 1;
    while (current != NULL) {
        printf("%d. [%d] %s\n", i, current->id, current->pilihan);
        current = current->next;
        i++;
    }
    if (i == 1) {
        printf("(Belum ada riwayat pilihan)\n");
    }
}

void clearRiwayat(RiwayatList* list) {
    Riwayat* current = list->head;
    while (current != NULL) {
        Riwayat* temp = current;
        current = current->next;
        free(temp);
    }
    list->head = NULL;
    list->tail = NULL;
}
