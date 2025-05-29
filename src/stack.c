#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

//Membuat stack kososng (top = NULL)
Stack* createStack() {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if (stack != NULL) {
        stack->top = NULL;
    }
    return stack;
}

//Mendorong node cerita saat ini ke stack (sebelum pindah ke scene baru)
void push(Stack* stack, TreeNode* scene) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    if (newNode != NULL) {
        newNode->scene = scene;
        newNode->next = stack->top;
        stack->top = newNode;
    }
}

// Mengambil node terakhir (undo)
TreeNode* pop(Stack* stack) {
    if (stack->top == NULL) {
        return NULL;  // Stack kosong
    }
    StackNode* temp = stack->top;
    TreeNode* scene = temp->scene;
    stack->top = temp->next;
    free(temp);
    return scene;
}

// Untuk mengecek stack kosong
int isEmptyStack(Stack* stack) {
    return stack->top == NULL;
}

// Untuk membersihkan semua node dari stack
void freeStack(Stack* stack) {
    while (!isEmptyStack(stack)) {
        pop(stack);  // pop otomatis free 1 node
    }
    free(stack);
}


