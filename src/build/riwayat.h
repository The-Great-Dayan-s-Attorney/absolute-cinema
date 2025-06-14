#ifndef RIWAYAT_H
#define RIWAYAT_H

#define MAX_TITLE 100

typedef struct Riwayat {
    int id;
    char pilihan[MAX_TITLE];
    struct Riwayat* next;
} Riwayat;

typedef struct {
    Riwayat* head;
    Riwayat* tail;
} RiwayatList;

// Inisialisasi list kosong
void initRiwayat(RiwayatList* list);

// Tambah riwayat pilihan ke akhir list
void addRiwayat(RiwayatList* list, int id, const char* pilihan);

// Menampilkan seluruh riwayat
void printRiwayat(RiwayatList* list);

// Membersihkan semua node riwayat
void clearRiwayat(RiwayatList* list);

#endif
