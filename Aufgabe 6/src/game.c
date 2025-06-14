#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

void print_connection_statistics(const GameBoard *board, long long total_games) {
    printf("\n=== DETAILED LADDER STATISTICS ===\n");
    int total_ladder_traversals = 0;
    for (int i = 0; i < board->ladder_count; i++) {
        total_ladder_traversals += board->ladders[i].traversed_count;
    }
    
    for (int i = 0; i < board->ladder_count; i++) {
        double game_frequency = total_games > 0 ? 
            (double)board->ladders[i].traversed_count / total_games * 100.0 : 0.0;
        double ladder_frequency = total_ladder_traversals > 0 ? 
            (double)board->ladders[i].traversed_count / total_ladder_traversals * 100.0 : 0.0;
        
        printf("Ladder %2d->%2d: ", board->ladders[i].from, board->ladders[i].to);
        printf("Used %4d times | ", board->ladders[i].traversed_count);
        printf("%.2f%% of games | ", game_frequency);
        printf("%.2f%% of all ladder uses\n", ladder_frequency);
    }
    printf("Total ladder traversals: %d\n", total_ladder_traversals);
    
    printf("\n=== DETAILED SNAKE STATISTICS ===\n");
    int total_snake_traversals = 0;
    for (int i = 0; i < board->snake_count; i++) {
        total_snake_traversals += board->snakes[i].traversed_count;
    }
    
    for (int i = 0; i < board->snake_count; i++) {
        double game_frequency = total_games > 0 ? 
            (double)board->snakes[i].traversed_count / total_games * 100.0 : 0.0;
        double snake_frequency = total_snake_traversals > 0 ? 
            (double)board->snakes[i].traversed_count / total_snake_traversals * 100.0 : 0.0;
        
        printf("Snake  %2d->%2d: ", board->snakes[i].from, board->snakes[i].to);
        printf("Used %4d times | ", board->snakes[i].traversed_count);
        printf("%.2f%% of games | ", game_frequency);
        printf("%.2f%% of all snake encounters\n", snake_frequency);
    }
    printf("Total snake traversals: %d\n", total_snake_traversals);
    
    printf("\n=== TRAVERSAL SUMMARY ===\n");
    printf("Average ladder uses per game: %.2f\n", 
           total_games > 0 ? (double)total_ladder_traversals / total_games : 0.0);
    printf("Average snake encounters per game: %.2f\n", 
           total_games > 0 ? (double)total_snake_traversals / total_games : 0.0);
}

void print_board_graph(const GameBoard *board) {
    printf("\n=== BOARD GRAPH STRUCTURE ===\n");
    printf("Board: %dx%d grid (%d squares)\n", board->rows, board->cols, board->total_squares);
    printf("Graph representation: Each square is a node with potential connections\n\n");
    
    printf("Connections in the graph:\n");
    printf("Square -> Destination (Type)\n");
    printf("─────────────────────────────\n");
    
    int connections_found = 0;
    for (int i = 1; i <= board->total_squares; i++) {
        if (board->nodes[i].has_connection) {
            printf("%3d    -> %3d       ", i, board->nodes[i].connection_to);
            if (board->nodes[i].connection_type == 1) {
                printf("(Ladder)\n");
            } else if (board->nodes[i].connection_type == -1) {
                printf("(Snake )\n");
            }
            connections_found++;
        }
    }
    
    if (connections_found == 0) {
        printf("No connections found in the graph.\n");
    }
    
    printf("\nGraph properties:\n");
    printf("- Total nodes: %d (squares 0-%d)\n", board->total_squares + 1, board->total_squares);
    printf("- Connected nodes: %d\n", connections_found);
    printf("- Unconnected nodes: %d\n", board->total_squares + 1 - connections_found);
    printf("- Ladders (positive connections): %d\n", board->ladder_count);
    printf("- Snakes (negative connections): %d\n", board->snake_count);
    
    // Show the board layout with connections marked
    printf("\nBoard layout (%dx%d):\n", board->rows, board->cols);
    for (int row = board->rows - 1; row >= 0; row--) {
        for (int col = 0; col < board->cols; col++) {
            int square;
            if (row % 2 == (board->rows - 1) % 2) {
                // Odd rows (from top): left to right
                square = row * board->cols + col + 1;
            } else {
                // Even rows (from top): right to left
                square = row * board->cols + (board->cols - col);
            }
            
            char marker = ' ';
            if (board->nodes[square].has_connection) {
                if (board->nodes[square].connection_type == 1) {
                    marker = 'L'; // Ladder
                } else if (board->nodes[square].connection_type == -1) {
                    marker = 'S'; // Snake
                }
            }
            
            printf("%3d%c", square, marker);
        }
        printf("\n");
    }
    printf("\nLegend: L=Ladder start, S=Snake head\n");
}

int parse_config_file(const char *filename, GameBoard *board) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening configuration file");
        return -1;
    }
    
    // Initialize board
    memset(board, 0, sizeof(GameBoard));
    board->die_sides = 6; // default
    board->num_simulations = 1000; // default
    board->nodes = NULL; // Will be allocated later
    
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Skip empty lines and comments
        if (strlen(line) <= 1 || line[0] == '#') {
            continue;
        }
        
        if (strncmp(line, "rows", 4) == 0) {
            sscanf(line + 4, "%d", &board->rows);
        }
        else if (strncmp(line, "cols", 4) == 0) {
            sscanf(line + 4, "%d", &board->cols);
        }
        else if (strncmp(line, "die", 3) == 0) {
            sscanf(line + 3, "%d", &board->die_sides);
        }
        else if (strncmp(line, "simulation", 10) == 0) {
            sscanf(line + 10, "%d", &board->num_simulations);
        }
        else if (strncmp(line, "ladder", 6) == 0) {
            int from, to;
            if (sscanf(line + 6, "%d %d", &from, &to) == 2) {
                if (board->ladder_count < MAX_CONNECTIONS) {
                    board->ladders[board->ladder_count].from = from;
                    board->ladders[board->ladder_count].to = to;
                    board->ladders[board->ladder_count].traversed_count = 0;
                    board->ladder_count++;
                }
            }
        }
        else if (strncmp(line, "snake", 5) == 0) {
            int from, to;
            if (sscanf(line + 5, "%d %d", &from, &to) == 2) {
                if (board->snake_count < MAX_CONNECTIONS) {
                    board->snakes[board->snake_count].from = from;
                    board->snakes[board->snake_count].to = to;
                    board->snakes[board->snake_count].traversed_count = 0;
                    board->snake_count++;
                }
            }
        }
    }
    
    fclose(file);
    
    board->total_squares = board->rows * board->cols;
    
    // Initialize the board graph
    if (initialize_board_graph(board) != 0) {
        return -1;
    }
    
    return validate_board(board);
}

int initialize_board_graph(GameBoard *board) {
    // Allocate memory for nodes (including position 0 for starting outside)
    board->nodes = malloc((board->total_squares + 1) * sizeof(GraphNode));
    if (!board->nodes) {
        fprintf(stderr, "Memory allocation failed for board graph\n");
        return -1;
    }
    
    // Initialize all nodes
    for (int i = 0; i <= board->total_squares; i++) {
        board->nodes[i].position = i;
        board->nodes[i].has_connection = 0;
        board->nodes[i].connection_to = i; // Default: connects to itself
        board->nodes[i].connection_type = 0; // No connection
    }
    
    // Build graph connections
    build_graph_connections(board);
    
    return 0;
}

void free_board_graph(GameBoard *board) {
    if (board->nodes) {
        free(board->nodes);
        board->nodes = NULL;
    }
}

void build_graph_connections(GameBoard *board) {
    // Add ladder connections to the graph
    for (int i = 0; i < board->ladder_count; i++) {
        int from = board->ladders[i].from;
        if (from >= 1 && from <= board->total_squares) {
            board->nodes[from].has_connection = 1;
            board->nodes[from].connection_to = board->ladders[i].to;
            board->nodes[from].connection_type = 1; // Ladder
        }
    }
    
    // Add snake connections to the graph
    for (int i = 0; i < board->snake_count; i++) {
        int from = board->snakes[i].from;
        if (from >= 1 && from <= board->total_squares) {
            board->nodes[from].has_connection = 1;
            board->nodes[from].connection_to = board->snakes[i].to;
            board->nodes[from].connection_type = -1; // Snake
        }
    }
}

int validate_board(const GameBoard *board) {
    if (board->rows <= 0 || board->cols <= 0) {
        fprintf(stderr, "Error: Invalid board dimensions\n");
        return -1;
    }
    
    if (board->die_sides <= 0) {
        fprintf(stderr, "Error: Invalid die configuration\n");
        return -1;
    }
    
    // Check ladder validity
    for (int i = 0; i < board->ladder_count; i++) {
        // Check bounds
        if (board->ladders[i].from < 1 || board->ladders[i].from >= board->total_squares || 
            board->ladders[i].to < 1 || board->ladders[i].to > board->total_squares) {
            fprintf(stderr, "Error: Ladder %d has positions out of bounds (%d->%d)\n", 
                    i, board->ladders[i].from, board->ladders[i].to);
            return -1;
        }
        
        // Check that ladder doesn't start at last square
        if (board->ladders[i].from == board->total_squares) {
            fprintf(stderr, "Error: Ladder %d starts at the last square (%d)\n", 
                    i, board->ladders[i].from);
            return -1;
        }
        
        // Check that ladder goes up (to > from)
        if (board->ladders[i].to <= board->ladders[i].from) {
            fprintf(stderr, "Error: Ladder %d must go up, but goes from %d to %d\n", 
                    i, board->ladders[i].from, board->ladders[i].to);
            return -1;
        }
        
        // Check that ladder doesn't start or end on itself
        if (board->ladders[i].from == board->ladders[i].to) {
            fprintf(stderr, "Error: Ladder %d starts and ends on the same square (%d)\n", 
                    i, board->ladders[i].from);
            return -1;
        }
    }
    
    // Check snake validity
    for (int i = 0; i < board->snake_count; i++) {
        // Check bounds
        if (board->snakes[i].from < 1 || board->snakes[i].from >= board->total_squares || 
            board->snakes[i].to < 1 || board->snakes[i].to > board->total_squares) {
            fprintf(stderr, "Error: Snake %d has positions out of bounds (%d->%d)\n", 
                    i, board->snakes[i].from, board->snakes[i].to);
            return -1;
        }
        
        // Check that snake doesn't start at last square
        if (board->snakes[i].from == board->total_squares) {
            fprintf(stderr, "Error: Snake %d starts at the last square (%d)\n", 
                    i, board->snakes[i].from);
            return -1;
        }
        
        // Check that snake goes down (to < from)
        if (board->snakes[i].to >= board->snakes[i].from) {
            fprintf(stderr, "Error: Snake %d must go down, but goes from %d to %d\n", 
                    i, board->snakes[i].from, board->snakes[i].to);
            return -1;
        }
        
        // Check that snake doesn't start or end on itself
        if (board->snakes[i].from == board->snakes[i].to) {
            fprintf(stderr, "Error: Snake %d starts and ends on the same square (%d)\n", 
                    i, board->snakes[i].from);
            return -1;
        }
    }
    
    // Check for overlapping start/end positions between all connections
    for (int i = 0; i < board->ladder_count; i++) {
        // Check ladder against other ladders
        for (int j = i + 1; j < board->ladder_count; j++) {
            if (board->ladders[i].from == board->ladders[j].from) {
                fprintf(stderr, "Error: Ladders %d and %d both start at square %d\n", 
                        i, j, board->ladders[i].from);
                return -1;
            }
            if (board->ladders[i].from == board->ladders[j].to) {
                fprintf(stderr, "Error: Ladder %d starts where ladder %d ends (square %d)\n", 
                        i, j, board->ladders[i].from);
                return -1;
            }
            if (board->ladders[i].to == board->ladders[j].from) {
                fprintf(stderr, "Error: Ladder %d ends where ladder %d starts (square %d)\n", 
                        i, j, board->ladders[i].to);
                return -1;
            }
            if (board->ladders[i].to == board->ladders[j].to) {
                fprintf(stderr, "Error: Ladders %d and %d both end at square %d\n", 
                        i, j, board->ladders[i].to);
                return -1;
            }
        }
        
        // Check ladder against snakes
        for (int j = 0; j < board->snake_count; j++) {
            if (board->ladders[i].from == board->snakes[j].from) {
                fprintf(stderr, "Error: Ladder %d and snake %d both start at square %d\n", 
                        i, j, board->ladders[i].from);
                return -1;
            }
            if (board->ladders[i].from == board->snakes[j].to) {
                fprintf(stderr, "Error: Ladder %d starts where snake %d ends (square %d)\n", 
                        i, j, board->ladders[i].from);
                return -1;
            }
            if (board->ladders[i].to == board->snakes[j].from) {
                fprintf(stderr, "Error: Ladder %d ends where snake %d starts (square %d)\n", 
                        i, j, board->ladders[i].to);
                return -1;
            }
            if (board->ladders[i].to == board->snakes[j].to) {
                fprintf(stderr, "Error: Ladder %d and snake %d both end at square %d\n", 
                        i, j, board->ladders[i].to);
                return -1;
            }
        }
    }
    
    // Check snakes against other snakes
    for (int i = 0; i < board->snake_count; i++) {
        for (int j = i + 1; j < board->snake_count; j++) {
            if (board->snakes[i].from == board->snakes[j].from) {
                fprintf(stderr, "Error: Snakes %d and %d both start at square %d\n", 
                        i, j, board->snakes[i].from);
                return -1;
            }
            if (board->snakes[i].from == board->snakes[j].to) {
                fprintf(stderr, "Error: Snake %d starts where snake %d ends (square %d)\n", 
                        i, j, board->snakes[i].from);
                return -1;
            }
            if (board->snakes[i].to == board->snakes[j].from) {
                fprintf(stderr, "Error: Snake %d ends where snake %d starts (square %d)\n", 
                        i, j, board->snakes[i].to);
                return -1;
            }
            if (board->snakes[i].to == board->snakes[j].to) {
                fprintf(stderr, "Error: Snakes %d and %d both end at square %d\n", 
                        i, j, board->snakes[i].to);
                return -1;
            }
        }
    }
    
    return 0;
}

int apply_connections(GameBoard *board, int position) {
    // Use the graph structure to find connections
    if (position >= 1 && position <= board->total_squares && 
        board->nodes[position].has_connection) {
        
        // Update traversal count based on connection type
        if (board->nodes[position].connection_type == 1) {
            // It's a ladder - find and update the corresponding ladder
            for (int i = 0; i < board->ladder_count; i++) {
                if (board->ladders[i].from == position) {
                    board->ladders[i].traversed_count++;
                    break;
                }
            }
        } else if (board->nodes[position].connection_type == -1) {
            // It's a snake - find and update the corresponding snake
            for (int i = 0; i < board->snake_count; i++) {
                if (board->snakes[i].from == position) {
                    board->snakes[i].traversed_count++;
                    break;
                }
            }
        }
        
        return board->nodes[position].connection_to;
    }
    
    return position; // No connection found
}

int simulate_single_game(GameBoard *board, int *rolls, int max_rolls) {
    int position = 0; // Start outside the board
    int roll_count = 0;
    
    while (position < board->total_squares && roll_count < max_rolls) {
        int roll = (rand() % board->die_sides) + 1;
        rolls[roll_count] = roll;
        roll_count++;
        
        int new_position = position + roll;
        
        // Check win condition - can exceed the last square
        if (new_position >= board->total_squares) {
            return roll_count;
        }
        
        // Apply snakes and ladders
        position = apply_connections(board, new_position);
    }
    
    return -1; // Timeout
}

void init_simulation_results(SimulationResults *results) {
    results->total_rolls = 0;
    results->shortest_rolls = INT_MAX;
    results->shortest_sequence = NULL;
    results->shortest_sequence_length = 0;
    results->total_games = 0;
    results->timeouts = 0;
}

void free_simulation_results(SimulationResults *results) {
    if (results->shortest_sequence) {
        free(results->shortest_sequence);
        results->shortest_sequence = NULL;
    }
}

void run_simulations(GameBoard *board, SimulationResults *results) {
    int *current_rolls = malloc(MAX_ROLLS * sizeof(int));
    if (!current_rolls) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    
    init_simulation_results(results);
    
    printf("Running %d simulations...\n", board->num_simulations);
    
    for (int i = 0; i < board->num_simulations; i++) {
        int rolls_needed = simulate_single_game(board, current_rolls, MAX_ROLLS);
        
        if (rolls_needed > 0) {
            results->total_rolls += rolls_needed;
            results->total_games++;
            
            // Check if this is the shortest game
            if (rolls_needed < results->shortest_rolls) {
                results->shortest_rolls = rolls_needed;
                results->shortest_sequence_length = rolls_needed;
                
                // Free previous sequence and allocate new one
                free(results->shortest_sequence);
                results->shortest_sequence = malloc(rolls_needed * sizeof(int));
                if (results->shortest_sequence) {
                    memcpy(results->shortest_sequence, current_rolls, rolls_needed * sizeof(int));
                }
            }
        } else {
            results->timeouts++;
        }
        
        // Progress indicator
        if ((i + 1) % 1000 == 0) {
            printf("Completed %d simulations\n", i + 1);
        }
    }
    
    free(current_rolls);
}

void print_statistics(const GameBoard *board, const SimulationResults *results) {
    printf("\n=== GAME CONFIGURATION ===\n");
    printf("Board: %d x %d (%d squares)\n", board->rows, board->cols, board->total_squares);
    printf("Die: %d sides\n", board->die_sides);
    printf("Simulations: %d\n", board->num_simulations);
    printf("Ladders: %d\n", board->ladder_count);
    printf("Snakes: %d\n", board->snake_count);
    
    printf("\n=== SIMULATION RESULTS ===\n");
    printf("Successful games: %lld\n", results->total_games);
    printf("Timeouts: %d\n", results->timeouts);
    
    if (results->total_games > 0) {
        double average_rolls = (double)results->total_rolls / results->total_games;
        printf("Average rolls to win: %.2f\n", average_rolls);
        printf("Shortest game: %d rolls\n", results->shortest_rolls);
        
        if (results->shortest_sequence && results->shortest_sequence_length > 0) {
            printf("Shortest sequence: ");
            for (int i = 0; i < results->shortest_sequence_length; i++) {
                printf("%d", results->shortest_sequence[i]);
                if (i < results->shortest_sequence_length - 1) printf(", ");
            }
            printf("\n");
        }
    }
    
    // Print detailed connection statistics
    print_connection_statistics(board, results->total_games);
    
    // Print the board graph structure
    print_board_graph(board);
}