#include "raylib.h"

int main(void) {
  SetConfigFlags(FLAG_WINDOW_HIGHDPI);
  InitWindow(800, 600, "MechBox2D");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);
    DrawFPS(0, 0);
    EndDrawing();
  }
  
  return 0;
}
