# Snakes and Ladders Simulator

This is a C-based simulator for the game **Snakes and Ladders**. It allows you to configure a custom board (rows, columns, ladders, snakes), define the number of die faces, and simulate thousands of games to collect statistics.

## Features

- Custom board dimensions and die configuration
- User-defined ladders and snakes
- Monte Carlo simulation of game outcomes
- Statistical reporting on ladder and snake usage

## Files

- `main.c`: Entry point that loads configuration, runs simulations, and prints results.
- `game.c`: Core game logic, including simulation, board traversal, and statistics.
- `game.h`: Header file with shared structures and function declarations.

## Compilation

Use `make` to compile:

```bash
make
```

This will produce an executable named `snakesAndLaddersSimulator`.

## Running the Simulator

```bash
./snakesAndLaddersSimulator boardLayouts/testboard1.txt
```

Replace `boardLayouts/testboard1.txt` with your configuration file.

## Configuration File Format

The config file is plain text with one command per line. Example:

```
rows 10
cols 10
die 6
simulation 1000
ladder 3 22
ladder 5 8
snake 97 78
snake 99 26
```

- `rows` / `cols`: Define the size of the board.
- `die`: Number of sides on the die.
- `simulation`: Number of games to simulate.
- `ladder <from> <to>`: A ladder from one square to another (up).
- `snake <from> <to>`: A snake from one square to another (down).

## Output

The program prints detailed statistics after simulations, including:

- Ladder and snake usage frequencies
- Total number of traversals
- Game-wide analysis of which paths are most common

