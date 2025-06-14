#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define MAX_CONNECTIONS 100
#define MAX_ROLLS 10000
#define MAX_LINE_LENGTH 256

/**
 * Structure representing a connection (ladder or snake)
 */
typedef struct {
    int from;
    int to;
    int traversed_count;
} Connection;

/**
 * Structure representing a node in the game board graph
 */
typedef struct {
    int position;
    int has_connection;
    int connection_to;
    int connection_type; // 0 = none, 1 = ladder, -1 = snake
} GraphNode;

/**
 * Structure representing the game board as a graph
 */
typedef struct {
    int rows;
    int cols;
    int total_squares;
    int die_sides;
    int num_simulations;
    Connection ladders[MAX_CONNECTIONS];
    Connection snakes[MAX_CONNECTIONS];
    int ladder_count;
    int snake_count;
    GraphNode *nodes; // Array of nodes representing the board graph
} GameBoard;

/**
 * Structure to store simulation results
 */
typedef struct {
    int total_rolls;
    int shortest_rolls;
    int *shortest_sequence;
    int shortest_sequence_length;
    long long total_games;
    int timeouts;
} SimulationResults;

/**
 * Parse a configuration file and initialize the game board
 * @param filename Path to the configuration file
 * @param board Pointer to GameBoard structure to initialize
 * @return 0 on success, -1 on error
 */
int parse_config_file(const char *filename, GameBoard *board);

/**
 * Initialize simulation results structure
 * @param results Pointer to SimulationResults structure
 */
void init_simulation_results(SimulationResults *results);

/**
 * Free memory allocated for simulation results
 * @param results Pointer to SimulationResults structure
 */
void free_simulation_results(SimulationResults *results);

/**
 * Simulate a single game of Snakes and Ladders
 * @param board Pointer to the game board
 * @param rolls Array to store the sequence of rolls
 * @param max_rolls Maximum number of rolls before timeout
 * @return Number of rolls to win, or -1 if timeout
 */
int simulate_single_game(GameBoard *board, int *rolls, int max_rolls);

/**
 * Run multiple simulations of the game
 * @param board Pointer to the game board
 * @param results Pointer to store simulation results
 */
void run_simulations(GameBoard *board, SimulationResults *results);

/**
 * Print detailed statistics from the simulation results
 * @param board Pointer to the game board
 * @param results Pointer to simulation results
 */
void print_statistics(const GameBoard *board, const SimulationResults *results);

/**
 * Get the destination square after applying snakes and ladders using graph
 * @param board Pointer to the game board
 * @param position Current position
 * @return Final position after applying connections
 */
int apply_connections(GameBoard *board, int position);

/**
 * Validate the game board configuration
 * @param board Pointer to the game board
 * @return 0 if valid, -1 if invalid
 */
int validate_board(const GameBoard *board);

/**
 * Print the board graph structure showing all connections
 * @param board Pointer to the game board
 */
void print_board_graph(const GameBoard *board);

/**
 * Initialize the game board graph structure
 * @param board Pointer to the game board
 * @return 0 on success, -1 on error
 */
int initialize_board_graph(GameBoard *board);

/**
 * Free memory allocated for the board graph
 * @param board Pointer to the game board
 */
void free_board_graph(GameBoard *board);

/**
 * Build the graph connections for snakes and ladders
 * @param board Pointer to the game board
 */
void build_graph_connections(GameBoard *board);

#endif /* GAME_H */