#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "interpreter.h"

// Function pointer type for instruction handlers
typedef void (*instruction_func_t)(struct state *state);

// Hash table entry structure
struct hash_entry {
    char instruction;
    instruction_func_t handler;
    const char* description;
    int is_occupied;
};

// Hash table size - using a prime number for better distribution
#define HASH_TABLE_SIZE 127

// Hash table structure
struct hash_table {
    struct hash_entry entries[HASH_TABLE_SIZE];
};

// Function declarations
void init_hash_table(void);
instruction_func_t get_instruction_handler(char instruction);
const char* get_instruction_description(char instruction);
void cleanup_hash_table(void);

// Instruction handler function declarations
void handle_nop(struct state *state);
void handle_left(struct state *state);
void handle_down(struct state *state);
void handle_up(struct state *state);
void handle_right(struct state *state);
void handle_jump_left(struct state *state);
void handle_jump_down(struct state *state);
void handle_jump_up(struct state *state);
void handle_jump_right(struct state *state);
void handle_turn_right(struct state *state);
void handle_turn_left(struct state *state);
void handle_end(struct state *state);
void handle_store_below(struct state *state);
void handle_store_above(struct state *state);
void handle_duplicate(struct state *state);
void handle_delete(struct state *state);
void handle_add_below(struct state *state);
void handle_add_above(struct state *state);
void handle_reduce_below(struct state *state);
void handle_reduce_above(struct state *state);
void handle_multiply_below(struct state *state);
void handle_multiply_above(struct state *state);
void handle_divide_below(struct state *state);
void handle_divide_above(struct state *state);
void handle_modulo_below(struct state *state);
void handle_modulo_above(struct state *state);
void handle_fetch_below(struct state *state);
void handle_fetch_above(struct state *state);
void handle_output_below(struct state *state);
void handle_output_above(struct state *state);
void handle_input_below(struct state *state);
void handle_input_above(struct state *state);

#endif // HASHTABLE_H