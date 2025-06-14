#include "game.h"

void print_usage(const char *program_name) {
    printf("Usage: %s <config_file>\n", program_name);
    printf("  config_file: Path to the game configuration file\n");
    printf("\nConfiguration file format:\n");
    printf("  rows <number>        - Number of rows on the board\n");
    printf("  cols <number>        - Number of columns on the board\n");
    printf("  die <number>         - Number of sides on the die\n");
    printf("  simulation <number>  - Number of simulations to run\n");
    printf("  ladder <from> <to>   - Add a ladder from square <from> to <to>\n");
    printf("  snake <from> <to>    - Add a snake from square <from> to <to>\n");
    printf("\nExample:\n");
    printf("  rows 10\n");
    printf("  cols 10\n");
    printf("  die 6\n");
    printf("  simulation 1000\n");
    printf("  ladder 5 10\n");
    printf("  snake 98 75\n");
}

int main(int argc, char *argv[]) {
    // Check command line arguments
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    // Seed random number generator
    srand((unsigned int)time(NULL));
    
    // Initialize game board
    GameBoard board;
    if (parse_config_file(argv[1], &board) != 0) {
        fprintf(stderr, "Failed to parse configuration file: %s\n", argv[1]);
        return 1;
    }
    
    // Run simulations
    SimulationResults results;
    run_simulations(&board, &results);
    
    // Print statistics
    print_statistics(&board, &results);
    
    // Clean up
    free_simulation_results(&results);
    free_board_graph(&board);
    
    return 0;
}