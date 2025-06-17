#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "riwayat.h"

void initRiwayat(RiwayatList* list) {
    list->first = NULL;
}

void addRiwayat(RiwayatList* list, int id, const char* pilihan) {
    addressRiwayat newNode = (addressRiwayat)malloc(sizeof(RiwayatNode));
    if (newNode == NULL) return;

    newNode->id = id;
    strncpy(newNode->pilihan, pilihan, MAX_TITLE - 1);
    newNode->pilihan[MAX_TITLE - 1] = '\0';
    newNode->next = list->first;
    list->first = newNode;
}

void clearRiwayat(RiwayatList* list) {
    addressRiwayat current = list->first;
    while (current != NULL) {
        addressRiwayat temp = current;
        current = current->next;
        free(temp);
    }
    list->first = NULL;
}

void printRiwayat(RiwayatList* list) {
    if (list->first == NULL) {
        printf("Tidak ada riwayat pilihan.\n");
        return;
    }

    printf("\n=== RIWAYAT PILIHAN ===\n");
    addressRiwayat current = list->first;
    int index = 1;
    while (current != NULL) {
        printf("%d. Pilihan %d: %s\n", index++, current->id, current->pilihan);
        current = current->next;
    }
}