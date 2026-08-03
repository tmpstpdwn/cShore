#include "raylib.h"
#include <stdlib.h>

#define FPS 60

#define SPEED 1

#define HEIGHT 600
#define WIDTH 800

// Particle square dimensions.
#define P_DIM 5
// Width and Height of the particle grid.
#define P_ARR_H (HEIGHT / P_DIM)
#define P_ARR_W (WIDTH / P_DIM)

typedef enum {
    NONE,
    SAND,
    WALL,
    PARTICLES, // Count.
} ParticleType;

static ParticleType particles[P_ARR_H][P_ARR_W];
static bool paused = false;

// Previous x, y for when the left mouse button was clicked.
static int prev_mouse_x = -1;
static int prev_mouse_y = -1;

static ParticleType curr_particle = SAND;

static void clear_particles(void) {
    for (int row = 0; row < P_ARR_H; row++) {
        for (int col = 0; col < P_ARR_W; col++) {
            particles[row][col] = NONE;
        }
    }
}

static char *get_tool_str(ParticleType tool) {
    switch (tool) {
    case NONE:
        return "ERASE";
    case SAND:
        return "SAND";
    case WALL:
        return "WALL";
    default:
        return "";
    }
}

static void set_cell(int row, int col, ParticleType tool) {
    if (col < 0 || col >= P_ARR_W || row < 0 || row >= P_ARR_H)
        return;
    switch (tool) {
    case NONE:
        particles[row][col] = NONE;
        break;
    case SAND:
        if (particles[row][col] == NONE)
            particles[row][col] = SAND;
        break;
    case WALL:
        if (particles[row][col] == NONE)
            particles[row][col] = WALL;
        break;
    default:;
    }
}

// Bresenham's line algorithm.
static void draw_line(int from_row, int from_col, int to_row, int to_col) {
    int dx = abs(to_col - from_col);
    int dy = abs(to_row - from_row);

    int scol = (from_col < to_col) ? 1 : -1;
    int srow = (from_row < to_row) ? 1 : -1;

    int error = dx - dy;

    while (true) {
        set_cell(from_row, from_col, curr_particle);

        if (from_col == to_col && from_row == to_row)
            break;

        int e2 = 2 * error;

        if (e2 > -dy) {
            error -= dy;
            from_col += scol;
        }

        if (e2 < dx) {
            error += dx;
            from_row += srow;
        }
    }
}

static void input(void) {
    float wheel = GetMouseWheelMove();

    if (wheel > 0) {
        curr_particle = (curr_particle + 1) % PARTICLES;
    } else if (wheel < 0) {
        curr_particle = (curr_particle == 0) ? PARTICLES - 1 : curr_particle - 1;
    }

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_L))
        clear_particles();

    if (IsKeyPressed(KEY_P))
        paused = !paused;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        int x = GetMouseX();
        int y = GetMouseY();

        int col = x / P_DIM;
        int row = y / P_DIM;

        if (prev_mouse_x == -1 || prev_mouse_y == -1) {
            set_cell(row, col, curr_particle);
        } else {
            int prev_col = prev_mouse_x / P_DIM;
            int prev_row = prev_mouse_y / P_DIM;
            draw_line(prev_row, prev_col, row, col);
        }

        prev_mouse_x = x;
        prev_mouse_y = y;
    } else {
        prev_mouse_x = -1;
        prev_mouse_y = -1;
    }
}

static void sim_particle(int row, int col) {
    if (particles[row][col] != SAND) return;

    // When cant move straight down.
    if (row + 1 < P_ARR_H && particles[row + 1][col] != NONE) {
        int new_col = col;

        if (col - 1 >= 0 && particles[row][col - 1] == NONE &&
            particles[row + 1][col - 1] == NONE)
            new_col = col - 1;
        else if (col + 1 < P_ARR_W &&
                 particles[row][col + 1] == NONE &&
                 particles[row + 1][col + 1] == NONE)
            new_col = col + 1;

        if (new_col != col) {
            particles[row + 1][new_col] = SAND;
            particles[row][col] = NONE;
        }
    }

    else { // Particle free fall.
        int new_row = row + SPEED;
        new_row = (new_row >= P_ARR_H) ? P_ARR_H - 1 : new_row;

        for (int i = row; i <= new_row; i++) {
            if (i == new_row || (i + 1 < new_row && particles[i + 1][col] != NONE)) {
                if (i != row) {
                    particles[i][col] = SAND;
                    particles[row][col] = NONE;
                }
                break;
            }
        }

    }
}

static void draw_particle(int row, int col) {
    int x = col * P_DIM;
    int y = row * P_DIM;

    switch (particles[row][col]) {
        case NONE:
            break;
        case SAND:
            DrawRectangle(x, y, P_DIM, P_DIM, (Color){226, 202, 118, 255});    
            break;
        case WALL:
            DrawRectangle(x, y, P_DIM, P_DIM, (Color){120, 120, 120, 255});
            break;
        default:;
    }
}

static void simulate(void) {
    if (paused) return;
    for (int row = P_ARR_H - 1; row >= 0; row--) {
        for (int col = P_ARR_W - 1; col >= 0; col--) {
            sim_particle(row, col);
        }
    }
}

static void draw(void) {
    BeginDrawing();

    DrawRectangle(0, 0, WIDTH, HEIGHT / 3,
                  (Color){170, 220, 255, 255});
    DrawRectangle(0, HEIGHT / 3, WIDTH, HEIGHT / 3,
                  (Color){120, 190, 250, 255});
    DrawRectangle(0, 2 * HEIGHT / 3, WIDTH, HEIGHT / 3,
                  (Color){70, 140, 220, 255});

    for (int row = P_ARR_H - 1; row >= 0; row--) {
        for (int col = P_ARR_W - 1; col >= 0; col--) {
            draw_particle(row, col);
        }
    }

    DrawText(get_tool_str(curr_particle), 0, 0, 30, RED);
    EndDrawing();
}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WIDTH, HEIGHT, "cSand");
    SetTargetFPS(FPS);

    while (!WindowShouldClose()) {
        input();
        simulate();
        draw();
    }

    CloseWindow();
    return 0;
}
