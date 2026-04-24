// Last Edited in Flulib 1.0

#include <math.h>
#include "flulib.h"

int main() {
    InitWindow(600, 600, "3D Example");
    SetTargetFPS(60);
    
    Camera cam = {0};
    cam.position = (Vector3){ 0.0f, 0.0f, -500.0f };
    cam.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    cam.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    cam.fovy = 45.0f;
    Vector3 cube[8] = {
        {-1,-1,-1}, { 1,-1,-1},
        { 1, 1,-1}, {-1, 1,-1},
        {-1,-1, 1}, { 1,-1, 1},
        { 1, 1, 1}, {-1, 1, 1}
    };
    
    int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };
    
    float angle = 0;
    
    while (!WindowShouldClose()) {
        angle += 0.02f;
        
        Vector3 transformed[8];

        for (int i = 0; i < 8; i++) {
            Vector3 p = cube[i];
            
            p = Vector3RotateByAxisAngle(p, (Vector3){ 1.0f, 0.0f, 0.0f }, angle);
            p = Vector3RotateByAxisAngle(p, (Vector3){ 0.0f, 1.0f, 0.0f }, angle * 0.7f);

            
            p.x *= 80;
            p.y *= 80;
            p.z *= 80;
            
            p.z += 300;
            
            transformed[i] = p;
        }
        
        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(cam);

            for (int i = 0; i < 12; i++) {
                DrawLine3D(
                    transformed[edges[i][0]],
                    transformed[edges[i][1]],
                    GREEN
                );
            }

            EndMode3D();

        EndDrawing();
    }

    return 0;
}