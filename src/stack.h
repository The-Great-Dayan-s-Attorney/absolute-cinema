#ifndef STACK_H
#define STACK_H

#include "tree.h"  // bisa akses TreeNode*

typedef struct StackNode {
    TreeNode *scene;
    struct StackNode *next;
} StackNode;

typedef struct {
    StackNode *top;
} Stack;

// Fungsi-fungsi Stack
Stack* createStack();
void push(Stack* stack, TreeNode* scene);
TreeNode* pop(Stack* stack);
int isEmptyStack(Stack* stack);
void freeStack(Stack* stack);

#endif
