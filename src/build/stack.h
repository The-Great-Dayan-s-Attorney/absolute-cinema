#ifndef STACK_H
#define STACK_H

#include "scene.h"

typedef struct StackNode {
    addressScene scene;
    struct StackNode *next;
} StackNode;

typedef struct {
    StackNode *top;
} Stack;

// Fungsi-fungsi Stack
Stack* createStack();
void push(Stack* stack, addressScene scene);
addressScene pop(Stack* stack);
int isEmptyStack(Stack* stack);
void freeStack(Stack* stack);

#endif
