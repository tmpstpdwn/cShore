#include <stdlib.h>

#include "raylib.h"

#define HEIGHT 600
#define WIDTH 800
#define FPS 60

// Particle square dimensions.
#define P_DIM 5

// Width and Height of the particle grid.
#define P_ARR_H (HEIGHT / P_DIM)
#define P_ARR_W (WIDTH / P_DIM)

#define PARTICLE_COUNT 4
#define COLORS_PER_P_TYPE 4

// Marker dimension limits in grid cells.
#define MARKER_MIN_SIZE 1
#define MARKER_MAX_SIZE 20

typedef enum {
    NONE,
    SAND,
    WATER,
    WALL
} ParticleType;

typedef struct {
    ParticleType type;
    int color_index;
} ParticleCell;
  
static ParticleCell particles[P_ARR_H][P_ARR_W];
static ParticleType curr_p_type = NONE;

static Color particle_colors[PARTICLE_COUNT][COLORS_PER_P_TYPE] = {
    [NONE] = {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    },
    [SAND] = {
        {194, 178, 128, 255},
        {210, 180, 140, 255},
        {180, 160, 110, 255},
        {220, 200, 150, 255}
    },
    [WATER] = {
        {0, 168, 232, 255},
        {0, 145, 199, 255},
        {0, 119, 182, 255},
        {72, 202, 228, 255}
    },
    [WALL] = {
        {170, 70, 45, 255},
        {200, 90, 50, 255},
        {140, 55, 40, 255},
        {220, 110, 65, 255}
    }
};

static int marker_r, marker_c;
static int marker_dim = 5;

// Previous row, col for the marker in the last frame.
// Used to draw continious line on the grid.
static int prev_marker_r = -1;
static int prev_marker_c = -1;

static bool paused = false;

static void clear_particles(void) {
    for (int row = 0; row < P_ARR_H; row++) {
        for (int col = 0; col < P_ARR_W; col++) {
            particles[row][col] = (ParticleCell){NONE, 0};
        }
    }
}

static char *get_type_str(ParticleType type) {
    switch (type) {
    case NONE:
        return "ERASE";
    case SAND:
        return "SAND";
    case WATER:
        return "WATER";
    case WALL:
        return "WALL";
    default:
        return "OOHLALA!";
    }
}

// Paint a square patch of side length `size` with particles of `type`.
static void paint_particles(int row, int col, int size, ParticleType type) {
    for (int r = row; r < row + size; r++) {
        for (int c = col; c < col + size; c++) {
            if (c < 0 || c >= P_ARR_W || r < 0 || r >= P_ARR_H)
                continue;

            if (type == NONE) {
                particles[r][c] = (ParticleCell){ NONE, 0 };
            } else if (particles[r][c].type == NONE) {
                int color_index = GetRandomValue(0, COLORS_PER_P_TYPE - 1);
                particles[r][c] = (ParticleCell){ type, color_index };
            }
        }
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
        paint_particles(from_row, from_col, marker_dim, curr_p_type);

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

    bool ctrl_down = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

    int x = GetMouseX();
    int y = GetMouseY();

    int new_r = (y - (marker_dim * P_DIM) / 2) / P_DIM;
    int new_c = (x - (marker_dim * P_DIM) / 2) / P_DIM;

    if (new_c < 0) new_c = 0;
    else if (new_c + marker_dim >= P_ARR_W) new_c = P_ARR_W - marker_dim;

    if (new_r < 0) new_r = 0;
    else if (new_r + marker_dim >= P_ARR_H) new_r = P_ARR_H - marker_dim;

    marker_r = new_r;
    marker_c = new_c;

    if (wheel > 0) {
        if (ctrl_down) {
            if (marker_dim < MARKER_MAX_SIZE) marker_dim++;
        } else {
            if (curr_p_type + 1 < PARTICLE_COUNT) curr_p_type++;
        }
    } else if (wheel < 0) {
        if (ctrl_down) {
            if (marker_dim > MARKER_MIN_SIZE) marker_dim--;
        } else {
            if ((int)curr_p_type - 1 >= NONE) curr_p_type--;
        }
    }

    if (ctrl_down && IsKeyPressed(KEY_L))
        clear_particles();

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        paused = !paused;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        if (prev_marker_c == -1 || prev_marker_r == -1) {
            paint_particles(marker_r, marker_c, marker_dim, curr_p_type);
        } else {
            draw_line(prev_marker_r, prev_marker_c, marker_r, marker_c);
        }
        prev_marker_r = marker_r;
        prev_marker_c = marker_c;
    } else {
        prev_marker_r = -1;
        prev_marker_c = -1;
    }
}

static void sim_particle(int row, int col, ParticleType type) {
    if (row + 1 >= P_ARR_H) return;
    if (type == NONE || type == WALL) return;

    int new_row = row;
    int new_col = col;

    bool left_col = col - 1 >= 0 && particles[row][col - 1].type == NONE;
    bool right_col = col + 1 < P_ARR_W && particles[row][col + 1].type == NONE;

    bool left_col_next_row = left_col && particles[row + 1][col - 1].type == NONE;
    bool right_col_next_row = right_col && particles[row + 1][col + 1].type == NONE;

    if (particles[row + 1][col].type == NONE) {
        new_row = row + 1;

        if (GetRandomValue(0, 10) < 1) {
            if (left_col_next_row && right_col_next_row) {
                if (GetRandomValue(0, 10) < 5) {
                    new_col = col - 1;
                } else {
                    new_col = col + 1;
                }
            }
            else if (left_col_next_row) {
                new_col = col - 1;
            }
            else if (right_col_next_row) {
                new_col = col + 1;
            }
        }        
    }

    else if (type == SAND) {
        if (left_col_next_row && right_col_next_row) {
            new_row = row + 1;
            if (GetRandomValue(0, 10) < 5) {
                new_col = col - 1;
            } else {
                new_col = col + 1;
            }
        }
        else if (left_col_next_row) {
            new_row = row + 1;
            new_col = col - 1;
        }
        else if (right_col_next_row) {
            new_row = row + 1;
            new_col = col + 1;
        }
    }

    else if (type == WATER) {
        if (left_col && right_col) {
            if (GetRandomValue(0, 10) < 5) {
                new_col = col - 1;
            } else {
                new_col = col + 1;
            }
        } else if (left_col) {
            new_col = col - 1;
        } else if (right_col) {
            new_col = col + 1;
        }

        if ((new_col == col - 1 && left_col_next_row) || (new_col == col + 1 && right_col_next_row)) {
            new_row = row + 1;
        }
    }

    if (new_row != row || new_col != col) {
        particles[new_row][new_col] = particles[row][col];
        particles[row][col] = (ParticleCell){NONE, 0};
    }
}

static void draw_particle(int row, int col) {
    int x = col * P_DIM;
    int y = row * P_DIM;

    ParticleCell pc = particles[row][col];

    if (pc.type != NONE) {
        DrawRectangle(x, y, P_DIM, P_DIM, particle_colors[pc.type][pc.color_index]);    
    }
}

static void simulate(void) {
    if (paused) return;

    // Flip direction every frame.
    static int dir = -1;

    for (int row = P_ARR_H - 1; row >= 0; row--) {
        int col_start = (dir == -1)? P_ARR_W - 1: 0;
        int col_end = (dir == -1)? 0: P_ARR_W - 1;
        col_end += dir;
        for (int col = col_start; col != col_end; col += dir) {
            ParticleType pt = particles[row][col].type;
            sim_particle(row, col, pt);
        }
    }

    dir = (dir == -1)? 1: -1;
}

static void draw(void) {
    BeginDrawing();

    DrawRectangle(0, 0, WIDTH, HEIGHT, (Color){170, 220, 255, 255});

    for (int row = 0; row < P_ARR_H; row++) {
        for (int col = 0; col < P_ARR_W; col++) {
            draw_particle(row, col);
        }
    }

    DrawText(TextFormat("%s %s", "TOOL:", get_type_str(curr_p_type)), 10, 10, 20, BLACK);
    DrawText(TextFormat("%s %d", "TOOL SIZE:", marker_dim), 10, 30, 20, BLACK);

    if (paused)
        DrawText("PAUSED", 705, 10, 20, RED);

    DrawRectangleLines(marker_c * P_DIM, marker_r * P_DIM, marker_dim * P_DIM, marker_dim * P_DIM, BLACK);

    EndDrawing();
}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WIDTH, HEIGHT, "cShore");
    SetTargetFPS(FPS);
    HideCursor();

    while (!WindowShouldClose()) {
        input();
        simulate();
        draw();
    }

    CloseWindow();
    return 0;
}
