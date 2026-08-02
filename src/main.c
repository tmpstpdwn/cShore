#include "raylib.h"
#include <stdlib.h>

#define FPS 60

#define ACCELERATION 3

#define HEIGHT 600
#define WIDTH 800

#define P_DIM 5
#define P_ARR_H (HEIGHT / P_DIM)
#define P_ARR_W (WIDTH / P_DIM)

static float particles[P_ARR_H][P_ARR_W];
static bool drawing = true;

static int prev_x = -1;
static int prev_y = -1;

int main(void) {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WIDTH, HEIGHT, "MechBox2D");
    SetTargetFPS(FPS);

    while (!WindowShouldClose()) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            int x = GetMouseX();
            int y = GetMouseY();

            int col = x / P_DIM;
            int row = y / P_DIM;

            if (prev_x == -1 || prev_y == -1) {
            } else {
                int prev_col = prev_x / P_DIM;
                int prev_row = prev_y / P_DIM;

                int dx = abs(col - prev_col);
                int dy = abs(row - prev_row);

                int scol = (prev_col < col) ? 1 : -1;
                int srow = (prev_row < row) ? 1 : -1;

                int error = dx - dy;

                while (true) {
                    if (prev_col >= 0 && prev_col < P_ARR_W && prev_row >= 0 && prev_row < P_ARR_H)
                        particles[prev_row][prev_col] = 1;

                    if (prev_col == col && prev_row == row)
                        break;

                    int e2 = 2 * error;

                    if (e2 > -dy) {
                        error -= dy;
                        prev_col += scol;
                    }

                    if (e2 < dx) {
                        error += dx;
                        prev_row += srow;
                    }

                }
            }

            prev_x = x;
            prev_y = y;
            drawing = true;
        } else {
            prev_x = -1;
            prev_y = -1;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            drawing = false;
        }

        BeginDrawing();
        ClearBackground(WHITE);

        for (int row = P_ARR_H - 1; row >= 0; row--) {
            for (int col = P_ARR_W - 1; col >= 0; col--) {

                if (particles[row][col] == 0)
                    continue;

                int x = col * P_DIM;
                int y = row * P_DIM;

                DrawRectangle(x, y, P_DIM, P_DIM, BLACK);

                if (drawing || row == P_ARR_H - 1)
                    continue;

                particles[row][col] += ACCELERATION * GetFrameTime();

                int new_row = row + (int)particles[row][col];

                if (new_row >= P_ARR_H)
                    new_row = P_ARR_H - 1;

                particles[new_row][col] = particles[row][col];
                particles[row][col] = 0;
            }
        }

        DrawFPS(0, 0);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
