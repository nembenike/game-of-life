#define _DEFAULT_SOURCE
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <X11/Xutil.h>
#include <stdlib.h>

#define SLEEP(ms) usleep((ms) * 1000)

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define GRID_WIDTH 50
#define GRID_HEIGHT 50

int *framebuffer;

typedef enum CellStatus
{
    ALIVE,
    DEAD
} CellStatus;

typedef CellStatus Grid[GRID_WIDTH][GRID_HEIGHT];

void clear_grid(Grid grid)
{
    for (int i = 0; i < GRID_WIDTH; i++)
    {
        for (int j = 0; j < GRID_HEIGHT; j++)
        {
            grid[i][j] = DEAD;
        }
    }
}

bool in_bounds(int row, int col)
{
    return row >= 0 && row < GRID_WIDTH &&
           col >= 0 && col < GRID_HEIGHT;
}

int cell_neighbor_count(Grid grid, int row, int col)
{
    int count = 0;

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
                continue;

            int r = row + i;
            int c = col + j;

            if (in_bounds(r, c) && grid[r][c] == ALIVE)
            {
                count++;
            }
        }
    }

    return count;
}

void pass_tick(Grid grid)
{
    Grid next;
    clear_grid(next);

    for (int i = 0; i < GRID_WIDTH; i++)
    {
        for (int j = 0; j < GRID_HEIGHT; j++)
        {
            int neighbors = cell_neighbor_count(grid, i, j);

            if (grid[i][j] == ALIVE)
            {
                if (neighbors == 2 || neighbors == 3)
                    next[i][j] = ALIVE;
            }
            else
            {
                if (neighbors == 3)
                    next[i][j] = ALIVE;
            }
        }
    }

    for (int i = 0; i < GRID_WIDTH; i++)
    {
        for (int j = 0; j < GRID_HEIGHT; j++)
        {
            grid[i][j] = next[i][j];
        }
    }
}

void fill_framebuffer(Grid grid)
{
    int cell_width = WINDOW_WIDTH / GRID_WIDTH;
    int cell_height = WINDOW_HEIGHT / GRID_HEIGHT;

    for (int i = 0; i < GRID_WIDTH; i++)
    {
        for (int j = 0; j < GRID_HEIGHT; j++)
        {
            int color = (grid[i][j] == ALIVE) ? 0xFFFFFFFF : 0x00000000;

            for (int x = 0; x < cell_width; x++)
            {
                for (int y = 0; y < cell_height; y++)
                {
                    int fb_x = i * cell_width + x;
                    int fb_y = j * cell_height + y;
                    framebuffer[fb_y * WINDOW_WIDTH + fb_x] = color;
                }
            }
        }
    }
}

int main()
{
    framebuffer = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(int));
    Grid grid;
    clear_grid(grid);

    grid[1][0] = ALIVE;
    grid[2][1] = ALIVE;
    grid[0][2] = ALIVE;
    grid[1][2] = ALIVE;
    grid[2][2] = ALIVE;

    Display *display = XOpenDisplay(NULL);
    Window window = XCreateSimpleWindow(
        display,
        XDefaultRootWindow(display), // parent
        0, 0,                        // x, y
        800, 600,                    // width, height
        0,                           // border width
        0x00000000,                  // border color
        0x00000000                   // background color
    );

    XStoreName(display, window, "Conway's Game of Life");
    XSelectInput(display, window, KeyPressMask | KeyReleaseMask);

    XMapWindow(display, window);

    XImage *image = XCreateImage(
        display,
        DefaultVisual(display, 0),
        DefaultDepth(display, 0),
        ZPixmap,
        0,
        (char *)framebuffer,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        32,
        0);

    bool quit = false;
    while (!quit)
    {
        while (XPending(display) > 0)
        {
            XEvent event = {0};
            XNextEvent(display, &event);
            if (event.type == KeyPress)
                quit = true;
        }

        pass_tick(grid);
        fill_framebuffer(grid);

        XPutImage(display, window, DefaultGC(display, 0), image,
                  0, 0, 0, 0,
                  WINDOW_WIDTH, WINDOW_HEIGHT);

        SLEEP(100);
    }

    image->data = NULL;
    XDestroyImage(image);
    free(framebuffer);

    XDestroyWindow(display, window);
    XCloseDisplay(display);
}
