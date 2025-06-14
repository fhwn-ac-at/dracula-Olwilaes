#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "interpreter.h"
#include "visualizer.h"
#include "hashTable.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <pfusch program> [--no-visual]\n", argv[0]);
        return 1;
    }

    // Check for visualization flag
    int visual_mode = 1;
    if (argc >= 3 && strcmp(argv[2], "--no-visual") == 0) {
        visual_mode = 0;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Error opening file");
        return 1;
    }

    // Initialize the hash table before using the interpreter
    init_hash_table();

    init_grid();
    load_program(fp);
    fclose(fp);

    // Initialize state
    struct state state = {0};
    state.stack.top = -1;
    state.ip.x = 0;
    state.ip.y = 0;
    state.ip.direction = RIGHT; // Default direction

    // Check if first instruction is a flow control instruction
    char first_instruction = grid[0][0];
    if (first_instruction != 'h' && first_instruction != 'j' && 
        first_instruction != 'k' && first_instruction != 'l' &&
        first_instruction != 'H' && first_instruction != 'J' &&
        first_instruction != 'K' && first_instruction != 'L') {
        fprintf(stderr, "Error: Program must start with a flow control instruction\n");
        cleanup_hash_table();
        return 1;
    }

    if (visual_mode) {
        // Visual execution loop
        printf("Starting Pfusch interpreter in visual mode...\n");
        printf("Press Ctrl+C to stop execution.\n\n");
        
        for (int steps = 0; steps < 10000; steps++) {  // Limit to prevent infinite loops
            print_visual_grid(&state);
            print_current_instruction_info(&state);
            execute_step(&state);
            usleep(100000);  // delay for better visualization
        }
        
        printf("\nExecution stopped after 10000 steps to prevent infinite loop.\n");
    } else {
        // Non-visual execution
        printf("Starting Pfusch interpreter...\n");
        
        for (int steps = 0; steps < 1000000; steps++) {  // Higher limit for non-visual
            execute_step(&state);
        }
        
        printf("\nExecution stopped after 1000000 steps to prevent infinite loop.\n");
    }

    // Clean up hash table before exiting
    cleanup_hash_table();
    return 0;
}