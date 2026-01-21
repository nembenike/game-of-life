#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define WIDTH 10
#define HEIGHT 10

typedef enum {
    ALIVE,
    DEAD
} Status;

typedef Status Grid[WIDTH][HEIGHT];

void print_grid(Grid grid) {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            if (grid[i][j] == DEAD) {
                printf("[ ]");
            } else {
                printf("[*]");
            }
        }
        printf("\n");
    }
}

void clear_grid(Grid grid) {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            grid[i][j] = DEAD;
        }
    }
}

bool in_bounds(int row, int col) {
    return (row >= 0 && row < WIDTH &&
            col >= 0 && col < HEIGHT);
}

int cell_neighbor_count(Grid grid, int row, int col) {
    int count = 0;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (!(i == 0 && j == 0) &&
                in_bounds(row + i, col + j) &&
                grid[row + i][col + j] == ALIVE) {
                count++;
            }
        }
    }

    return count;
}

void pass_tick(Grid grid) {
    Grid next;

    clear_grid(next);

    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            int neighbors = cell_neighbor_count(grid, i, j);

            if (grid[i][j] == ALIVE) {
                if (neighbors == 2 || neighbors == 3) {
                    next[i][j] = ALIVE;
                } else {
                    next[i][j] = DEAD;
                }
            } else {
                if (neighbors == 3) {
                    next[i][j] = ALIVE;
                }
            }
        }
    }

    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            grid[i][j] = next[i][j];
        }
    }
}

int main(void) {
    Grid grid;

    clear_grid(grid);

    grid[4][5] = ALIVE;
    grid[4][6] = ALIVE;
    grid[5][4] = ALIVE;
    grid[5][5] = ALIVE;
    grid[6][5] = ALIVE;

    print_grid(grid);
    
    while (1) {
        pass_tick(grid);
        printf("\033[H\033[J");
        print_grid(grid);
        sleep(1);
    }

    return 0;
}
