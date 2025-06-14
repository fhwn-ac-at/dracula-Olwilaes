#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdio.h>

#define GRID_HEIGHT 42
#define GRID_WIDTH 69
#define STACK_SIZE 1000

// Global grid
extern int grid[GRID_HEIGHT][GRID_WIDTH];

// Stack structure
struct stack {
    int top;
    int data[STACK_SIZE];
};

// Direction enumeration
enum direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// Instruction pointer structure
struct instructionPointer {
    int x;
    int y;
    enum direction direction;
};

// Complete program state
struct state {
    struct stack stack;
    struct instructionPointer ip;
};

// Function declarations
void init_grid(void);
void load_program(FILE *fp);
void execute_step(struct state *state);

#endif