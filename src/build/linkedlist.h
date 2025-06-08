#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>

// Node untuk linked list pilihan player
typedef struct ListNode {
    int choice;               // Pilihan yang diambil player (misal: 1, 2, atau 3)
    struct ListNode *next;
} ListNode;

// Linked list untuk menyimpan jalur pilihan
typedef struct {
    ListNode *head;
} LinkedList;

// Fungsi-fungsi linked list
LinkedList* createLinkedList();
void insertNode(LinkedList *list, int choice);
void displayList(const LinkedList *list);
void freeList(LinkedList *list);

#endif
