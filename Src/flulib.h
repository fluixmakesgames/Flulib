// Flulib 1.0
// Under ZLib License

#include <stdbool.h>
#include <math.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define DrawText FluDrawText
#define Rectangle FluRectangle
#define CloseWindow FluCloseWindow

// Types
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
}Color;

// 2D
typedef struct {
    float x;
    float y;
}Vector2;

typedef struct {
    float x;
    float y;
    float width;
    float height;
}FluRectangle;

// 3D
typedef struct {
    float x;
    float y;
    float z;
}Vector3;

// Camera
typedef struct {
    Vector3 position;
    Vector3 target;
    Vector3 up;
    float fovy;
    int projection;
}Camera;


Camera currentCamera;
#define CAMERA_PERSPECTIVE 0

// Colours
#define RAYWHITE   RGB(245, 245, 245)
#define LIGHTGRAY  RGB(200, 200, 200)
#define GRAY       RGB(130, 130, 130)
#define DARKGRAY   RGB(80, 80, 80)
#define YELLOW     RGB(253, 249, 0)
#define GOLD       RGB(255, 203, 0)
#define ORANGE     RGB(255, 161, 0)
#define PINK       RGB(255, 109, 194)
#define RED        RGB(230, 41, 55)
#define MAROON     RGB(190, 33, 55)
#define GREEN      RGB(0, 228, 48)
#define LIME       RGB(0, 158, 47)
#define DARKGREEN  RGB(0, 117, 44)
#define BLUE       RGB(0, 121, 241)
#define DARKBLUE   RGB(0, 82, 172)
#define PURPLE     RGB(200, 122, 255)
#define VIOLET     RGB(135, 60, 190)
#define DARKPURPLE RGB(112, 31, 126)
#define BEIGE      RGB(211, 176, 131)
#define BROWN      RGB(127, 106, 79)
#define DARKBROWN  RGB(76, 63, 47)
#define WHITE      RGB(255, 255, 255)
#define BLACK      RGB(0, 0, 0)
#define MAGENTA    RGB(255, 0, 255)

HWND hwnd;
HDC globalDC;
bool close_bool = false;

void FluCloseWindow() {
    DestroyWindow(hwnd);
}


HDC memDC;
HBITMAP memBitmap;
HBITMAP oldBitmap;
RECT clientRect;

LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    switch(msg) {
        case WM_CLOSE:   DestroyWindow(h); return 0;
        case WM_DESTROY: PostQuitMessage(0); return 0;
        case WM_ERASEBKGND: return 1; // Prevents flickering
    }
    return DefWindowProc(h, msg, wp, lp);
}

bool currKeys[256] = {0};
bool prevKeys[256] = {0};

void InitWindow(int width, int height, const char* title) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "FluWin";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    hwnd = CreateWindow("FluWin", title, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
                        CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hInst, 0);
}

int targetFrameTime = 0;
DWORD frameStart;
float lastFrameTime = 0.0f;

void SetTargetFPS(int fps) {
    targetFrameTime = 500 / fps;
}

void UpdateKeysInternal() {
    for (int i = 0; i < 256; i++) {
        prevKeys[i] = currKeys[i];
        currKeys[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }
}

bool WindowShouldClose() {
    MSG msg;
    
    UpdateKeysInternal();
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) close_bool = true;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return close_bool;
}

void BeginDrawing() {
    HDC screenDC = GetDC(hwnd);
    GetClientRect(hwnd, &clientRect);
    memDC = CreateCompatibleDC(screenDC);
    memBitmap = CreateCompatibleBitmap(screenDC, clientRect.right, clientRect.bottom);
    oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
    globalDC = memDC;
    ReleaseDC(hwnd, screenDC);
    frameStart = GetTickCount();
}

void EndDrawing() {
    HDC screenDC = GetDC(hwnd);
    BitBlt(screenDC, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
    ReleaseDC(hwnd, screenDC);
    DWORD frameTime = GetTickCount() - frameStart;
    lastFrameTime = (float)frameTime;
    
    if (frameTime < targetFrameTime) {
        Sleep(targetFrameTime - frameTime);
    }
}

void ClearBackground(COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(globalDC, &clientRect, brush);
    DeleteObject(brush);
}

void DrawRectangle(float xf, float yf, int w, int h, COLORREF color) {
    int x = (int)xf;
    int y = (int)yf;
    HBRUSH brush = CreateSolidBrush(color);
    RECT r = { x, y, x + w, y + h };
    FillRect(globalDC, &r, brush);
    DeleteObject(brush);
}

void FluDrawText(const char* text, float xf, float yf, int fontSize, COLORREF color) {
    int x = (int)xf;
    int y = (int)yf;
    SetTextColor(globalDC, color);
    SetBkMode(globalDC, TRANSPARENT);
    
    HFONT font = CreateFontA(fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
                             DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "MS Gothic");
    
    HFONT oldFont = (HFONT)SelectObject(globalDC, font);
    
    TextOutA(globalDC, x, y, text, lstrlenA(text));
    
    SelectObject(globalDC, oldFont);
    DeleteObject(font);
}

void DrawFPS(int x, int y) {
    float fps = 0;

    fps = (lastFrameTime);
    
    char buffer[32];
    wsprintfA(buffer, "FPS: %i", fps);

    FluDrawText(buffer, x, y, 20, GREEN);
}

bool IsKeyDown(int key) {
    return currKeys[key];
}

bool IsKeyPressed(int key) {
    return currKeys[key] && !prevKeys[key];
}

#define KEY_LEFT VK_LEFT
#define KEY_RIGHT VK_RIGHT
#define KEY_UP VK_UP
#define KEY_DOWN VK_DOWN

bool CheckCollisionRecs(Rectangle r1, Rectangle r2) {
    return (r1.x < r2.x + r2.width  &&
            r1.x + r1.width > r2.x  &&
            r1.y < r2.y + r2.height &&
            r1.y + r1.height > r2.y);
}

// Camera Functions
void BeginMode3D(Camera cam) {
    currentCamera = cam;
}

void EndMode3D() {
    return;
}

Vector3 ApplyCamera(Vector3 p, Camera cam) {
    p.x -= cam.position.x;
    p.y -= cam.position.y;
    p.z -= cam.position.z;
    return p;
}

Vector2 Project3D(Vector3 p, Camera cam) {
    p = ApplyCamera(p, cam);

    float z = p.z;
    if (z <= 0.1f) z = 0.1f;

    float fovRad = cam.fovy * (3.14159f / 180.0f);
    float scale = 1.0f / tanf(fovRad / 2.0f);

    Vector2 result;
    result.x = (p.x * scale) / z * (clientRect.right / 2) + (clientRect.right / 2);
    result.y = (p.y * scale) / z * (clientRect.bottom / 2) + (clientRect.bottom / 2);

    return result;
}

void DrawLine3D(Vector3 a, Vector3 b, COLORREF color) {
    Vector2 p1 = Project3D(a, currentCamera);
    Vector2 p2 = Project3D(b, currentCamera);
    
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    HPEN oldPen = (HPEN)SelectObject(globalDC, pen);
    
    MoveToEx(globalDC, (int)p1.x, (int)p1.y, NULL);
    LineTo(globalDC, (int)p2.x, (int)p2.y);
    
    SelectObject(globalDC, oldPen);
    DeleteObject(pen);
}

Vector3 Vector3RotateX(Vector3 p, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);

    float y = p.y * c - p.z * s;
    float z = p.y * s + p.z * c;

    p.y = y;
    p.z = z;
    return p;
}

Vector3 Vector3RotateY(Vector3 p, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);

    float x = p.x * c + p.z * s;
    float z = -p.x * s + p.z * c;

    p.x = x;
    p.z = z;
    return p;
}

Vector3 Vector3RotateZ(Vector3 p, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);

    float x = p.x * c - p.y * s;
    float y = p.x * s + p.y * c;

    p.x = x;
    p.y = y;
    return p;
}

Vector3 Vector3RotateByAxisAngle(Vector3 v, Vector3 axis, float angle) {
    if (axis.x == 1.0f) return Vector3RotateX(v, angle);
    if (axis.y == 1.0f) return Vector3RotateY(v, angle);
    if (axis.z == 1.0f) return Vector3RotateZ(v, angle);
    return v;
}