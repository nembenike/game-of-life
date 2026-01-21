#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdbool.h>

#ifdef _WIN32
#include <Windows.h>
#define SLEEP(ms) Sleep(ms)
#else
#include <unistd.h>
#define SLEEP(ms) usleep((ms) * 1000)
#endif

#define WIDTH 10
#define HEIGHT 10

typedef enum {
    ALIVE,
    DEAD
} Status;

typedef Status Grid[WIDTH][HEIGHT];

void move_cursor_home(void) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { 0, 0 };
    SetConsoleCursorPosition(hConsole, coord);
#else
    printf("\033[H");
#endif
}

void hide_cursor(void) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &info);
#else
    printf("\033[?25l");
#endif
}

void clear_grid(Grid grid) {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            grid[i][j] = DEAD;
        }
    }
}

void print_grid(Grid grid) {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            printf(grid[i][j] == ALIVE ? "*" : " ");
        }
        printf("\n");
    }
}

bool in_bounds(int row, int col) {
    return row >= 0 && row < WIDTH &&
        col >= 0 && col < HEIGHT;
}

int cell_neighbor_count(Grid grid, int row, int col) {
    int count = 0;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0)
                continue;

            int r = row + i;
            int c = col + j;

            if (in_bounds(r, c) && grid[r][c] == ALIVE) {
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
                if (neighbors == 2 || neighbors == 3)
                    next[i][j] = ALIVE;
            }
            else {
                if (neighbors == 3)
                    next[i][j] = ALIVE;
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
    int generation = 0;

    clear_grid(grid);

    grid[4][5] = ALIVE;
    grid[4][6] = ALIVE;
    grid[5][4] = ALIVE;
    grid[5][5] = ALIVE;
    grid[6][5] = ALIVE;

    hide_cursor();

    while (1) {
        move_cursor_home();

        print_grid(grid);
        printf("\nGeneration: %d\n", generation);

        pass_tick(grid);
        generation++;

        fflush(stdout);
        SLEEP(500);
    }

    return 0;
}
