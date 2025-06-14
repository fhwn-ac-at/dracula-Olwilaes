#include "interpreter.h"
#include "hashTable.h"
#include "visualizer.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Global grid definition
int grid[GRID_HEIGHT][GRID_WIDTH];

void init_grid(void) {
    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            grid[i][j] = ' ';
        }
    }
}

void load_program(FILE *fp) {
    char line[GRID_WIDTH + 2];  // Extra space for newline and null terminator
    for (int row = 0; row < GRID_HEIGHT; row++) {
        if (!fgets(line, sizeof(line), fp))
            break;

        for (int col = 0; col < GRID_WIDTH && line[col] != '\n' && line[col] != '\0'; col++) {
            unsigned char ch = (unsigned char)line[col];
            // Accept 7-bit ASCII characters
            if (ch <= 127) {
                grid[row][col] = ch;
            } else {
                fprintf(stderr, "Error: Invalid character at line %d, column %d: ASCII %d (must be 7-bit ASCII)\n", 
                       row + 1, col + 1, ch);
                exit(1);
            }
        }
    }
}

void turnLeft(struct instructionPointer *ip) {
    switch (ip->direction) {
        case UP: ip->direction = LEFT; break;
        case LEFT: ip->direction = DOWN; break;
        case DOWN: ip->direction = RIGHT; break;
        case RIGHT: ip->direction = UP; break;
    }
}

void turnRight(struct instructionPointer *ip) {
    switch (ip->direction) {
        case UP: ip->direction = RIGHT; break;
        case RIGHT: ip->direction = DOWN; break;
        case DOWN: ip->direction = LEFT; break;
        case LEFT: ip->direction = UP; break;
    }
}

// Stack operations
int stack_push(struct stack *s, int value) {
    if (s->top >= STACK_SIZE - 1) {
        fprintf(stderr, "Error: Stack overflow\n");
        return -1;
    }
    s->data[++s->top] = value;
    return 0;
}

int stack_pop(struct stack *s, int *value) {
    if (s->top < 0) {
        fprintf(stderr, "Error: Stack underflow - cannot pop from empty stack\n");
        return -1;
    }
    *value = s->data[s->top--];
    return 0;
}

int stack_peek(struct stack *s, int *value) {
    if (s->top < 0) {
        fprintf(stderr, "Error: Stack is empty - cannot peek\n");
        return -1;
    }
    *value = s->data[s->top];
    return 0;
}

// Helper function to get cell value with bounds checking
int get_cell_value(int x, int y) {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
        fprintf(stderr, "Error: Accessing cell outside bounds (%d, %d)\n", x, y);
        exit(1);
    }
    return grid[y][x];
}

// Helper function to set cell value with bounds checking
void set_cell_value(int x, int y, int value) {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
        fprintf(stderr, "Error: Setting cell outside bounds (%d, %d)\n", x, y);
        exit(1);
    }
    grid[y][x] = value;
}

// Jump functions
int jump_in_direction(struct instructionPointer *ip, enum direction dir, int target_value) {
    int x = ip->x;
    int y = ip->y;
    
    // Move in the specified direction until we find the target or hit boundary
    while (1) {
        switch (dir) {
            case LEFT: x--; break;
            case RIGHT: x++; break;
            case UP: y--; break;
            case DOWN: y++; break;
        }
        
        // Check bounds
        if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
            return -1; // Target not found
        }
        
        if (grid[y][x] == target_value) {
            ip->x = x;
            ip->y = y;
            ip->direction = dir;
            return 0; // Found target
        }
    }
}

void execute_step(struct state *state) {
    char current_instruction = grid[state->ip.y][state->ip.x];
    
    // Check if character is valid 7-bit ASCII
    if ((unsigned char)current_instruction > 127) {
        fprintf(stderr, "Error: Invalid instruction at (%d, %d): ASCII %d (must be 7-bit ASCII)\n", 
               state->ip.x, state->ip.y, (unsigned char)current_instruction);
        exit(1);
    }
    
    // Try to get instruction handler from hash table
    instruction_func_t handler = get_instruction_handler(current_instruction);
    
    if (handler != NULL) {
        // Execute the instruction using the function pointer
        handler(state);
        
        // Special case: Jump instructions handle their own IP movement
        if (current_instruction == 'H' || current_instruction == 'J' || 
            current_instruction == 'K' || current_instruction == 'L') {
            return; // Don't move IP again
        }
    } else {
        // Character is not a valid instruction - treat as no-op for valid ASCII
        // This handles digits, letters, and other printable characters
        if ((unsigned char)current_instruction < 32) {
            fprintf(stderr, "Error: Invalid instruction at (%d, %d): ASCII %d (control character)\n", 
                   state->ip.x, state->ip.y, (unsigned char)current_instruction);
            exit(1);
        }
        // Valid printable ASCII characters that aren't instructions are treated as no-ops
    }
    
    // Move instruction pointer in current direction
    switch (state->ip.direction) {
        case UP:
            if (state->ip.y <= 0) {
                fprintf(stderr, "Error: Instruction pointer moved outside bounds (UP)\n");
                exit(1);
            }
            state->ip.y--;
            break;
        case DOWN:
            if (state->ip.y >= GRID_HEIGHT - 1) {
                fprintf(stderr, "Error: Instruction pointer moved outside bounds (DOWN)\n");
                exit(1);
            }
            state->ip.y++;
            break;
        case LEFT:
            if (state->ip.x <= 0) {
                fprintf(stderr, "Error: Instruction pointer moved outside bounds (LEFT)\n");
                exit(1);
            }
            state->ip.x--;
            break;
        case RIGHT:
            if (state->ip.x >= GRID_WIDTH - 1) {
                fprintf(stderr, "Error: Instruction pointer moved outside bounds (RIGHT)\n");
                exit(1);
            }
            state->ip.x++;
            break;
    }
}