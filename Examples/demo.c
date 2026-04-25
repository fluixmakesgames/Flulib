// Created in Flulib 1.0
// Last edited in Flulib 1.1

#define FL_NO_3D
#define FL_NO_2D
#define FL_NO_TEXTURES
#include "flulib.h"

int main() {
    InitWindow(1200, 500, "Flulib Demo.");
    
    int ticks = 0;
    int seconds = 0;
    const char *text = "Hello! Flulib is a tiny library inspired by Raylib! Flulib can do quite a few things like... Text!";
    char buffer[255];
    
    while (!WindowShouldClose()) {
        ticks++;
        
        if (ticks % 10 == 0 && seconds < lstrlenA(text)) {
            seconds++;
        }
        
        lstrcpynA(buffer, text, seconds + 1);
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
            
            DrawText(buffer, 20, 20, 15, BLACK);
        EndDrawing();
        Sleep(1);
    }
    return 0;
}
