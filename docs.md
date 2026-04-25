# Flulib 1.1 Documentation
> A tiny, fast, header-only 2D/3D graphics library for Windows, inspired by Raylib.

---

## What is Flulib?
Flulib is a single-header C library that lets you open windows, draw 2D/3D graphics, handle input, and load textures — all without any external dependencies beyond what Windows already provides. It is heavily inspired by Raylib's API, meaning many Raylib programs can run on Flulib with little or no changes.

Flulib is **not** a full Raylib replacement. It is intentionally tiny and Windows-only for now, with Linux and Web ports planned for the future.

---

## Building
Flulib requires no special build system. Just include `flulib.h` and compile with GCC (MinGW):

```bash
gcc myprogram.c -o myprogram -lgdi32 -lmsimg32 -lm
```

If you use `fltx.h` (textured rendering) you need `stb_image.h` in your include path.

---

## Flag System
Flulib supports compile-time flags to strip out unused features and reduce binary size. Define flags **before** including `flulib.h`.

### Opt-Out Flags (default: everything on)
| Flag | Effect |
|------|--------|
| `FL_NO_2D` | Disables 2D drawing (rectangles, shapes, etc) |
| `FL_NO_TEXT` | Disables `DrawText` and `DrawFPS` |
| `FL_NO_3D` | Disables all 3D rendering |
| `FL_NO_TEXTURES` | Disables texture loading and drawing |

### Example — Tiny 3D only binary:
```c
#define FL_NO_2D
#define FL_NO_TEXTURES
#include "flulib.h"
```

### Example — Text only (smallest useful binary ~16KB):
```c
#define FL_NO_2D
#define FL_NO_3D
#define FL_NO_TEXTURES
#include "flulib.h"
```

---

## Separate Headers
| Header | Description | Requires |
|--------|-------------|---------|
| `flulib.h` | Core library | `gdi32`, `msimg32` |
| `flmath.h` | Vector math, BoundingBox, collision | nothing |
| `fltx.h` | Textured 3D rendering | `flulib.h`, `stb_image.h` |

---

## Core
### Window
```c
void InitWindow(int width, int height, const char* title);
bool WindowShouldClose();
void CloseWindow();
```

### Drawing Loop
```c
void BeginDrawing();
void EndDrawing();
void ClearBackground(COLORREF color);
```

### Timing
```c
void SetTargetFPS(int fps);
float GetFrameTime(); // returns time in seconds, like Raylib
```

### Always Available
```c
void DrawPixel(int x, int y, COLORREF color);
```

---

## Colors
Flulib uses `COLORREF` (Windows RGB) for colors. The following named colors are always available:

```c
RAYWHITE, WHITE, BLACK, GRAY, DARKGRAY, LIGHTGRAY
RED, MAROON, GREEN, LIME, DARKGREEN
BLUE, DARKBLUE, YELLOW, GOLD, ORANGE
PINK, PURPLE, VIOLET, DARKPURPLE
BEIGE, BROWN, DARKBROWN, MAGENTA
```

---

## Input
```c
bool IsKeyDown(int key);      // held this frame
bool IsKeyPressed(int key);   // just pressed this frame
```

### Key Constants
```c
KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN
KEY_W, KEY_A, KEY_S, KEY_D
KEY_Q, KEY_E
// Standard VK_ codes from windows.h also work
```

---

## 2D Drawing
> Requires: `FL_NO_2D` not defined

```c
void DrawRectangle(float x, float y, int width, int height, COLORREF color);
bool CheckCollisionRecs(FluRectangle r1, FluRectangle r2);
```

### Types
```c
typedef struct { float x, y, width, height; } FluRectangle;
```

---

## Text
> Requires: `FL_NO_TEXT` not defined

```c
void DrawText(const char* text, float x, float y, int fontSize, COLORREF color);
void DrawFPS(int x, int y);
```

---

## Textures
> Requires: `FL_NO_TEXTURES` not defined

```c
Texture2D LoadTexture(const char* path); // supports .bmp and .png
void DrawTexture(Texture2D tex, int x, int y, COLORREF tint);
void DrawTextureEx(Texture2D tex, int x, int y, int width, int height, COLORREF tint);
void DrawTextureRec(Texture2D tex, FluRectangle src, int x, int y, COLORREF tint);
void UnloadTexture(Texture2D tex);
```

### Type
```c
typedef struct {
    HBITMAP hbm;
    HDC     hdc;
    int     width;
    int     height;
    int     hasAlpha;
} Texture2D;
```

### Notes
- PNG files with transparency are automatically detected and use alpha blending
- Fully opaque images use the fast `BitBlt` path
- Use `WHITE` as tint for no tinting

---

## 3D Rendering
> Requires: `FL_NO_3D` not defined

### Camera
```c
void BeginMode3D(Camera cam);
void EndMode3D();             // flushes and sorts all queued triangles
```

```c
typedef struct {
    Vector3 position;
    Vector3 target;
    Vector3 up;
    float   fovy;
    int     projection;
} Camera;

#define CAMERA_PERSPECTIVE 0
```

### Drawing
```c
void DrawLine3D(Vector3 a, Vector3 b, COLORREF color);
void DrawTriangle3D(Vector3 a, Vector3 b, Vector3 c, COLORREF color);
void DrawCube(Vector3 center, float width, float height, float depth, COLORREF color);
void DrawCubeWires(Vector3 center, float width, float height, float depth, COLORREF color);
```

### Notes
- Triangles are depth sorted (painter's algorithm) and backface culled automatically
- `EndMode3D` is when triangles actually get drawn — don't skip it!
- Camera `target` and `up` are fully respected

---

## Math (flmath.h)
```c
// Vector3
Vector3 Vector3Add(Vector3 a, Vector3 b);
Vector3 Vector3Subtract(Vector3 a, Vector3 b);
Vector3 Vector3Scale(Vector3 v, float s);
Vector3 Vector3Normalize(Vector3 v);
Vector3 Vector3Lerp(Vector3 a, Vector3 b, float t);
float   Vector3Length(Vector3 v);
float   Vector3DotProduct(Vector3 a, Vector3 b);
Vector3 Vector3CrossProduct(Vector3 a, Vector3 b);

// Rotation
Vector3 Vector3RotateX(Vector3 p, float angle);
Vector3 Vector3RotateY(Vector3 p, float angle);
Vector3 Vector3RotateZ(Vector3 p, float angle);
Vector3 Vector3RotateByAxisAngle(Vector3 v, Vector3 axis, float angle);

// Collision
typedef struct { Vector3 min; Vector3 max; } BoundingBox;
bool        CheckCollisionBoxes(BoundingBox a, BoundingBox b);
BoundingBox GetCubeBoundingBox(Vector3 center, float w, float h, float d);
```

---

## Textured 3D (fltx.h) — FL Extended
> Requires: `flulib.h`, `stb_image.h`
> Note: This is a Flulib extension — not part of the standard Raylib API.

```c
void DrawCubeTextured(Vector3 center, float width, float height, float depth, Texture2D tex);
void PushTexturedTriangle3D(Vector3 a, Vector3 b, Vector3 c,
                             Texture2D tex,
                             float u1, float v1,
                             float u2, float v2,
                             float u3, float v3);
```

### Notes
- `DrawCubeTextured` applies the same texture to all 6 faces
- Rendering is per-pixel (`SetPixel`) so performance degrades with large on-screen size
- Marked as **FL Extended** — not present in Raylib by default

---

## Planned Features
- **Cross Platform** (Linux + Web ports planned for 1.2+)
- **Faster Textured 3D** right now it uses per-pixel `SetPixel` which QUICKLY gets slow, optimization planned.
- **Audio**
- **Mouse input**
- **Better Depth Testing** depth sorting uses Painter's Algorithim which can artifact with intersecting geometry.
- `DrawCubeTextured` face UVs may appear inverted on some faces in 1.1

---

## Planned for 1.2
- Linux port (X11 backend)
- Mouse input
- `ShowMessageBox`
- `GetFileDialog` / `SaveFileDialog`
- `flgui.h` — basic UI widgets (button, slider, text input, checkbox)
- `flgl.h` — low level rendering (rlgl replacement)
- Optimized textured rendering

---

*Flulib is under the ZLib License.*