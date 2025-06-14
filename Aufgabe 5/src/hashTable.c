#include "hashTable.h"
#include "visualizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Global hash table instance
static struct hash_table instruction_table;

// Hash function
static int hash_function(char key) {
    return ((unsigned char)key) % HASH_TABLE_SIZE;
}

// Insert instruction into hash table
static void insert_instruction(char instruction, instruction_func_t handler, const char* description) {
    int index = hash_function(instruction);
    int original_index = index;
    
    // Linear probing to handle collisions
    while (instruction_table.entries[index].is_occupied) {
        index = (index + 1) % HASH_TABLE_SIZE;
        if (index == original_index) {
            fprintf(stderr, "Error: Hash table is full\n");
            exit(1);
        }
    }
    
    instruction_table.entries[index].instruction = instruction;
    instruction_table.entries[index].handler = handler;
    instruction_table.entries[index].description = description;
    instruction_table.entries[index].is_occupied = 1;
}

// Lookup instruction in hash table
static struct hash_entry* lookup_instruction(char instruction) {
    int index = hash_function(instruction);
    int original_index = index;
    
    // Find the instruction
    while (instruction_table.entries[index].is_occupied) {
        if (instruction_table.entries[index].instruction == instruction) {
            return &instruction_table.entries[index];
        }
        index = (index + 1) % HASH_TABLE_SIZE;
        if (index == original_index) {
            break; // Searched entire table
        }
    }
    
    return NULL; // Not found
}

// Initialize hash table with all instructions
void init_hash_table(void) {
    // Clear the hash table
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        instruction_table.entries[i].is_occupied = 0;
    }
    
    // Insert all instructions
    insert_instruction('#', handle_nop, "no operation (NOP)");
    insert_instruction('h', handle_left, "set execution direction to left");
    insert_instruction('j', handle_down, "set execution direction to down");
    insert_instruction('k', handle_up, "set execution direction to up");
    insert_instruction('l', handle_right, "set execution direction to right");
    insert_instruction('H', handle_jump_left, "jump left to next cell matching top of stack; set direction to left");
    insert_instruction('J', handle_jump_down, "jump down to next cell matching top of stack; set direction to down");
    insert_instruction('K', handle_jump_up, "jump up to next cell matching top of stack; set direction to up");
    insert_instruction('L', handle_jump_right, "jump right to next cell matching top of stack; set direction to right");
    insert_instruction('x', handle_turn_right, "if top of stack > 0, turn direction 90 degrees right");
    insert_instruction('X', handle_turn_left, "if top of stack < 0, turn direction 90 degrees left");
    insert_instruction('e', handle_end, "end program execution");
    insert_instruction('s', handle_store_below, "push value of cell below current to stack");
    insert_instruction('S', handle_store_above, "push value of cell above current to stack");
    insert_instruction('d', handle_duplicate, "duplicate top value on stack");
    insert_instruction('D', handle_delete, "delete top value off stack");
    insert_instruction('a', handle_add_below, "add value of cell below to top of stack");
    insert_instruction('A', handle_add_above, "add value of cell above to top of stack");
    insert_instruction('r', handle_reduce_below, "reduce top of stack by value of cell below");
    insert_instruction('R', handle_reduce_above, "reduce top of stack by value of cell above");
    insert_instruction('p', handle_multiply_below, "multiply top of stack by value of cell below");
    insert_instruction('P', handle_multiply_above, "multiply top of stack by value of cell above");
    insert_instruction('q', handle_divide_below, "divide top of stack by value of cell below (quotient)");
    insert_instruction('Q', handle_divide_above, "divide top of stack by value of cell above (quotient)");
    insert_instruction('m', handle_modulo_below, "divide top of stack by value of cell below (remainder)");
    insert_instruction('M', handle_modulo_above, "divide top of stack by value of cell above (remainder)");
    insert_instruction('f', handle_fetch_below, "pop top of stack and store to cell below");
    insert_instruction('F', handle_fetch_above, "pop top of stack and store to cell above");
    insert_instruction('o', handle_output_below, "send value of cell below to stdout");
    insert_instruction('O', handle_output_above, "send value of cell above to stdout");
    insert_instruction('i', handle_input_below, "read one byte from stdin and store in cell below");
    insert_instruction('I', handle_input_above, "read one byte from stdin and store in cell above");
}

// Get instruction handler function pointer
instruction_func_t get_instruction_handler(char instruction) {
    struct hash_entry* entry = lookup_instruction(instruction);
    return entry ? entry->handler : NULL;
}

// Get instruction description
const char* get_instruction_description(char instruction) {
    struct hash_entry* entry = lookup_instruction(instruction);
    if (entry) {
        return entry->description;
    }
    
    // Handle characters not in the instruction set
    static char description_buffer[150];
    unsigned char ch = (unsigned char)instruction;
    
    // Check if it's in valid ASCII range for Pfusch (7-bit ASCII)
    if (ch > 127) {
        snprintf(description_buffer, sizeof(description_buffer), 
                "Invalid character: ASCII %d (outside valid 7-bit ASCII range)", ch);
        return description_buffer;
    }
    
    // Handle all other valid ASCII characters as no-ops
    if (isprint(ch)) {
        snprintf(description_buffer, sizeof(description_buffer), 
                "No operation (character '%c' skipped)", ch);
    } else {
        snprintf(description_buffer, sizeof(description_buffer), 
                "No operation (character ASCII %d skipped)", ch);
    }
    
    return description_buffer;
}

// Cleanup hash table
void cleanup_hash_table(void) {
    // Clear the hash table
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        instruction_table.entries[i].is_occupied = 0;
    }
}

// Helper functions (from interpreter.c)
extern void turnLeft(struct instructionPointer *ip);
extern void turnRight(struct instructionPointer *ip);
extern int stack_push(struct stack *s, int value);
extern int stack_pop(struct stack *s, int *value);
extern int stack_peek(struct stack *s, int *value);
extern int get_cell_value(int x, int y);
extern void set_cell_value(int x, int y, int value);
extern int jump_in_direction(struct instructionPointer *ip, enum direction dir, int target_value);

void handle_nop(struct state *state) {
    (void)state; // Suppress unused parameter warning
    // No operation
}

void handle_left(struct state *state) {
    state->ip.direction = LEFT;
}

void handle_down(struct state *state) {
    state->ip.direction = DOWN;
}

void handle_up(struct state *state) {
    state->ip.direction = UP;
}

void handle_right(struct state *state) {
    state->ip.direction = RIGHT;
}

void handle_jump_left(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) != 0) exit(1);
    if (jump_in_direction(&state->ip, LEFT, value) != 0) {
        fprintf(stderr, "Error: Jump target not found\n");
        exit(1);
    }
}

void handle_jump_down(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) != 0) exit(1);
    if (jump_in_direction(&state->ip, DOWN, value) != 0) {
        fprintf(stderr, "Error: Jump target not found\n");
        exit(1);
    }
}

void handle_jump_up(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) != 0) exit(1);
    if (jump_in_direction(&state->ip, UP, value) != 0) {
        fprintf(stderr, "Error: Jump target not found\n");
        exit(1);
    }
}

void handle_jump_right(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) != 0) exit(1);
    if (jump_in_direction(&state->ip, RIGHT, value) != 0) {
        fprintf(stderr, "Error: Jump target not found\n");
        exit(1);
    }
}

void handle_turn_right(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) == 0 && value > 0) {
        turnRight(&state->ip);
    }
}

void handle_turn_left(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) == 0 && value < 0) {
        turnLeft(&state->ip);
    }
}

void handle_end(struct state *state) {
    (void)state; // Suppress unused parameter warning
    printf("\nProgram ended normally.\n");
    exit(0);
}

void handle_store_below(struct state *state) {
    int below_value = get_cell_value(state->ip.x, state->ip.y + 1);
    if (stack_push(&state->stack, below_value) != 0) exit(1);
}

void handle_store_above(struct state *state) {
    int above_value = get_cell_value(state->ip.x, state->ip.y - 1);
    if (stack_push(&state->stack, above_value) != 0) exit(1);
}

void handle_duplicate(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) != 0) exit(1);
    if (stack_push(&state->stack, value) != 0) exit(1);
}

void handle_delete(struct state *state) {
    int value;
    if (stack_pop(&state->stack, &value) != 0) exit(1);
}

void handle_add_below(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) != 0) exit(1);
    int below_value = get_cell_value(state->ip.x, state->ip.y + 1);
    state->stack.data[state->stack.top] = value + below_value;
}

void handle_add_above(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) != 0) exit(1);
    int above_value = get_cell_value(state->ip.x, state->ip.y - 1);
    state->stack.data[state->stack.top] = value + above_value;
}

void handle_reduce_below(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) != 0) exit(1);
    int below_value = get_cell_value(state->ip.x, state->ip.y + 1);
    state->stack.data[state->stack.top] = value - below_value;
}

void handle_reduce_above(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) != 0) exit(1);
    int above_value = get_cell_value(state->ip.x, state->ip.y - 1);
    state->stack.data[state->stack.top] = value - above_value;
}

void handle_multiply_below(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) != 0) exit(1);
    int below_value = get_cell_value(state->ip.x, state->ip.y + 1);
    state->stack.data[state->stack.top] = value * below_value;
}

void handle_multiply_above(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) != 0) exit(1);
    int above_value = get_cell_value(state->ip.x, state->ip.y - 1);
    state->stack.data[state->stack.top] = value * above_value;
}

void handle_divide_below(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) != 0) exit(1);
    int below_value = get_cell_value(state->ip.x, state->ip.y + 1);
    if (below_value == 0) {
        fprintf(stderr, "Error: Division by zero\n");
        exit(1);
    }
    state->stack.data[state->stack.top] = value / below_value;
}

void handle_divide_above(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) != 0) exit(1);
    int above_value = get_cell_value(state->ip.x, state->ip.y - 1);
    if (above_value == 0) {
        fprintf(stderr, "Error: Division by zero\n");
        exit(1);
    }
    state->stack.data[state->stack.top] = value / above_value;
}

void handle_modulo_below(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) != 0) exit(1);
    int below_value = get_cell_value(state->ip.x, state->ip.y + 1);
    if (below_value == 0) {
        fprintf(stderr, "Error: Division by zero\n");
        exit(1);
    }
    state->stack.data[state->stack.top] = value % below_value;
}

void handle_modulo_above(struct state *state) {
    int value;
    if (stack_peek(&state->stack, &value) != 0) exit(1);
    int above_value = get_cell_value(state->ip.x, state->ip.y - 1);
    if (above_value == 0) {
        fprintf(stderr, "Error: Division by zero\n");
        exit(1);
    }
    state->stack.data[state->stack.top] = value % above_value;
}

void handle_fetch_below(struct state *state) {
    int value;
    if (stack_pop(&state->stack, &value) != 0) exit(1);
    set_cell_value(state->ip.x, state->ip.y + 1, value);
}

void handle_fetch_above(struct state *state) {
    int value;
    if (stack_pop(&state->stack, &value) != 0) exit(1);
    set_cell_value(state->ip.x, state->ip.y - 1, value);
}

void handle_output_below(struct state *state) {
    int below_value = get_cell_value(state->ip.x, state->ip.y + 1);
    // Check for valid 7-bit ASCII (0-127)
    if (below_value < 0 || below_value > 127) {
        fprintf(stderr, "Error: Invalid ASCII value for output: %d (must be 0-127)\n", below_value);
        exit(1);
    }
    putchar(below_value);
    add_to_output(below_value);
    fflush(stdout);
}

void handle_output_above(struct state *state) {
    int above_value = get_cell_value(state->ip.x, state->ip.y - 1);
    // Check for valid 7-bit ASCII (0-127)
    if (above_value < 0 || above_value > 127) {
        fprintf(stderr, "Error: Invalid ASCII value for output: %d (must be 0-127)\n", above_value);
        exit(1);
    }
    putchar(above_value);
    add_to_output(above_value);
    fflush(stdout);
}

void handle_input_below(struct state *state) {
    int value = getchar();
    if (value == EOF) value = 0;
    set_cell_value(state->ip.x, state->ip.y + 1, value);
}

void handle_input_above(struct state *state) {
    int value = getchar();
    if (value == EOF) value = 0;
    set_cell_value(state->ip.x, state->ip.y - 1, value);
}