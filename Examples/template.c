// Last Edited in Flulib 1.0

#include "flulib.h"

int main() {
    InitWindow(1200, 500, "Flulib Demo.");
    while (!WindowShouldClose()) {
        
        lstrcpynA(buffer, text, seconds + 1);
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("First Window!", 20, 20, 15, BLACK);
        EndDrawing();
    }
    return 0;
}
