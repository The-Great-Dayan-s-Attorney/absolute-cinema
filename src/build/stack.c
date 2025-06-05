#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "scene.h"

//Membuat stack kosong (top = NULL)
Stack* createStack() {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if (stack != NULL) {
        stack->top = NULL;
    }
    return stack;
}

//Menambahkan scene ke atas stack
void push(Stack* stack, addressScene scene) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    if (newNode != NULL) {
        newNode->scene = scene;
        newNode->next = stack->top;
        stack->top = newNode;
    }
}

// Menghapus dan mengembalikan scene teratas dari stack (undo)
addressScene pop(Stack* stack) {
    if (stack->top == NULL) {
        return NULL;  // Stack kosong
    }
    StackNode* temp = stack->top;
    addressScene scene = temp->scene;
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
        pop(stack);  
    }
    free(stack);
}


