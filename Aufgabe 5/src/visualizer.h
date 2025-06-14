#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "interpreter.h"

// Function declarations for visualization
void clear_screen(void);
void print_visual_grid(struct state *state);
void print_current_instruction_info(struct state *state);
void add_to_output(char c);

#endif // VISUALIZER_H