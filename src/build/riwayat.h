#ifndef RIWAYAT_H
#define RIWAYAT_H

#include "convention.h"

typedef struct RiwayatNode* addressRiwayat;
typedef struct RiwayatNode {
    int id;
    char pilihan[MAX_TITLE];
    addressRiwayat next;
} RiwayatNode;

typedef struct {
    addressRiwayat first;
} RiwayatList;

void initRiwayat(RiwayatList* list);
void addRiwayat(RiwayatList* list, int id, const char* pilihan);
void clearRiwayat(RiwayatList* list);
void printRiwayat(RiwayatList* list);

#endif