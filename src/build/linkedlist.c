#include "linkedlist.h"

// Membuat linked list kosong (head = NULL)
LinkedList* createLinkedList() {
    LinkedList *list = (LinkedList*) malloc(sizeof(LinkedList));
    if (list != NULL) {
        list->head = NULL;
    } else {
        fprintf(stderr, "Error: Gagal alokasi memory untuk linked list.\n");
    }
    return list;
}

// Menambahkan node baru ke akhir list (pilihan player)
void insertNode(LinkedList *list, int choice) {
    ListNode *newNode = (ListNode*) malloc(sizeof(ListNode));
    if (newNode == NULL) {
        fprintf(stderr, "Error: Gagal alokasi memory untuk node linked list.\n");
        return;
    }
    newNode->choice = choice;
    newNode->next = NULL;

    if (list->head == NULL) {
        // Jika list kosong, node baru jadi head
        list->head = newNode;
    } else {
        // Cari node terakhir
        ListNode *temp = list->head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

// Menampilkan isi jalur pilihan (contoh output: "Pilihan: 1 -> 3 -> 2")
void displayList(const LinkedList *list) {
    if (list->head == NULL) {
        printf("Belum ada pilihan.\n");
        return;
    }

    ListNode *temp = list->head;
    printf("Jalur Pilihan: ");
    while (temp != NULL) {
        printf("%d", temp->choice);
        if (temp->next != NULL) {
            printf(" -> ");
        }
        temp = temp->next;
    }
    printf("\n");
}

// Membebaskan semua node dan linked list-nya
void freeList(LinkedList *list) {
    ListNode *temp = list->head;
    while (temp != NULL) {
        ListNode *next = temp->next;
        free(temp);
        temp = next;
    }
    free(list);
}
