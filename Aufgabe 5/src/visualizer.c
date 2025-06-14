#include "visualizer.h"
#include "hashTable.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Global output buffer to capture program output
static char output_buffer[1000] = {0};
static int output_pos = 0;

void add_to_output(char c) {
    if (output_pos < (int)(sizeof(output_buffer) - 1)) {
        output_buffer[output_pos++] = c;
        output_buffer[output_pos] = '\0';
    }
}

void clear_screen(void) {
    printf("\033[2J\033[H");
}

// Helper function to determine if character should be displayed as printable
int is_displayable_char(unsigned char ch) {
    return (ch >= 32 && ch <= 126);
}

char get_display_char(unsigned char ch) {
    if (is_displayable_char(ch)) {
        return ch;
    } else {
        return '#';  // Display unprintable characters as '#'
    }
}

void print_visual_grid(struct state *state) {
    clear_screen();
    
    const char* direction_names[] = {"UP", "DOWN", "LEFT", "RIGHT"};
    
    // Top border
    printf("┌");
    for (int i = 0; i < GRID_WIDTH + 4; i++) printf("─");  // grid width + "yy | "
    printf("┬");
    for (int i = 0; i < 25; i++) printf("─");
    printf("┐\n");
    
    // Grid rows with right panel
    for (int row = 0; row < GRID_HEIGHT; row++) {
        printf("│%2d │", row);
        
        // Print grid content
        for (int col = 0; col < GRID_WIDTH; col++) {
            unsigned char ch = (unsigned char)grid[row][col];
            char display_ch = get_display_char(ch);
            
            // Highlight current IP position
            if (row == state->ip.y && col == state->ip.x) {
                printf("\033[7m%c\033[0m", display_ch);
            } else {
                printf("%c", display_ch);
            }
        }
        
        printf("│");
        
        // Right panel content
        if (row == 1) {
            printf(" IP Position:          ");
        } else if (row == 2) {
            printf(" (%2d, %2d)              ", state->ip.x, state->ip.y);
        } else if (row == 3) {
            printf(" Direction: %-10s ", direction_names[state->ip.direction]);
        } else if (row == 4) {
            printf("───────────────────────");
        } else if (row == 5) {
            printf(" Stack (top to bottom):");
        } else if (row >= 6 && row <= 6 + state->stack.top && state->stack.top >= 0) {
            int stack_index = state->stack.top - (row - 6);
            if (stack_index >= 0) {
                printf(" [%2d]: %-12d  ", stack_index, state->stack.data[stack_index]);
            } else {
                printf("                       ");
            }
        } else if (row == 6 && state->stack.top < 0) {
            printf(" (empty)               ");
        } else {
            printf("                       ");
        }
        
        printf("│\n");
    }
    
    // Middle border
    printf("├");
    for (int i = 0; i < GRID_WIDTH + 4; i++) printf("─");
    printf("┴");
    for (int i = 0; i < 25; i++) printf("─");
    printf("┤\n");
    
// Output section - 10 lines high
for (int output_row = 0; output_row < 10; output_row++) {
    if (output_row == 0) {
        printf("│ Output:%-*s│\n", GRID_WIDTH + 4 + 25 - 8, "");
    } else if (output_row == 1) {
        // Display raw output buffer with actual newlines and formatting
        printf("│ ");
        int chars_printed = 2; // Account for "│ "
        int max_width = GRID_WIDTH + 4 + 25 - 2; // Total width minus borders
        
        for (int i = 0; i < output_pos && chars_printed < max_width - 1; i++) {
            if (output_buffer[i] == '\n') {
                // Fill rest of current line and start new output row
                for (int pad = chars_printed; pad < max_width - 1; pad++) {
                    printf(" ");
                }
                printf("│\n");
                output_row++;
                if (output_row >= 10) break;
                printf("│ ");
                chars_printed = 2;
            } else if (is_displayable_char((unsigned char)output_buffer[i])) {
                printf("%c", output_buffer[i]);
                chars_printed++;
            } else {
                // Show unprintable characters as #
                printf("#");
                chars_printed++;
            }
        }
        // Fill rest of line
        for (int pad = chars_printed; pad < max_width - 1; pad++) {
            printf(" ");
        }
        printf("│\n");
    } else {
        printf("│%-*s│\n", GRID_WIDTH + 4 + 25, "");
    }
}
    
    // Bottom border
    printf("└");
    for (int i = 0; i < GRID_WIDTH + 4 + 25; i++) printf("─");
    printf("┘\n");
}

void print_current_instruction_info(struct state *state) {
    unsigned char current_instruction = (unsigned char)grid[state->ip.y][state->ip.x];
    char display_ch = get_display_char(current_instruction);
    
    printf("Current instruction at (%d, %d): '%c' (ASCII %d) - ", 
           state->ip.x, state->ip.y, display_ch, current_instruction);
    printf("%s\n", get_instruction_description(current_instruction));
}