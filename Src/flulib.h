// Flulib 1.1
// Under ZLib License

#include <stdbool.h>
#include <math.h>

#include <windows.h>

#ifndef FL_NO_TEXTURES
    #define STB_IMAGE_IMPLEMENTATION
    #include "stb_image.h"
#endif

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

#ifndef FL_NO_TEXTURES
typedef struct {
    HBITMAP hbm;
    HDC     hdc;
    int     width;
    int     height;
    int id;
    int format;
} Texture2D;
int TextureCount = 0;
#endif

// 2D

typedef struct {
    float x;
    float y;
}Vector2;

#ifndef FL_NO_2D
typedef struct {
    float x;
    float y;
    float width;
    float height;
}FluRectangle;
#endif

// 3D
#ifndef FL_NO_3D
typedef struct {
    float x;
    float y;
    float z;
}Vector3;

typedef struct {
    Vector3 min;
    Vector3 max;
} BoundingBox;

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
#endif

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
    
    HDC screenDC = GetDC(hwnd);
    GetClientRect(hwnd, &clientRect);
    memDC = CreateCompatibleDC(screenDC);
    memBitmap = CreateCompatibleBitmap(screenDC, width, height);
    oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
    globalDC = memDC;
    
    SelectObject(memDC, GetStockObject(DC_BRUSH));
    SelectObject(memDC, GetStockObject(DC_PEN));
    
    ReleaseDC(hwnd, screenDC);
}

LARGE_INTEGER qpcFreq;
LARGE_INTEGER qpcStart;
int targetFrameTime = 0;
float lastFrameTime = 0.0f;

void SetTargetFPS(int fps) {
    QueryPerformanceFrequency(&qpcFreq);
    targetFrameTime = 1000 / fps;
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

float GetFrameTime() {
    return lastFrameTime / 1000.0f;
}

void BeginDrawing() {
    GetClientRect(hwnd, &clientRect);
    QueryPerformanceCounter(&qpcStart);
}

void EndDrawing() {
    HDC screenDC = GetDC(hwnd);
    BitBlt(screenDC, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);
    ReleaseDC(hwnd, screenDC);
    
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    float frameMs = (float)(now.QuadPart - qpcStart.QuadPart) * 1000.0f / (float)qpcFreq.QuadPart;
    lastFrameTime = frameMs;
    
    if (frameMs < targetFrameTime - 2.0f)
        Sleep((DWORD)(targetFrameTime - frameMs - 2.0f));
    do {
        QueryPerformanceCounter(&now);
    } while ((float)(now.QuadPart - qpcStart.QuadPart) * 1000.0f / (float)qpcFreq.QuadPart < targetFrameTime);
    
    QueryPerformanceCounter(&now);
    frameMs = (float)(now.QuadPart - qpcStart.QuadPart) * 1000.0f / (float)qpcFreq.QuadPart;
    lastFrameTime = frameMs;
}

void ClearBackground(COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(globalDC, &clientRect, brush);
    DeleteObject(brush);
}

#ifndef FL_NO_2D
void DrawRectangle(float xf, float yf, int w, int h, COLORREF color) {
    int x = (int)xf;
    int y = (int)yf;
    RECT r = { x, y, x + w, y + h };
    SelectObject(globalDC, GetStockObject(DC_BRUSH));
    SetDCBrushColor(globalDC, color);
    FillRect(globalDC, &r, GetStockObject(DC_BRUSH));
}
#endif

// Text
#ifndef FL_NO_TEXT
void FluDrawText(const char* text, float xf, float yf, int fontSize, COLORREF color) {
    int x = (int)xf;
    int y = (int)yf;
    
    SetTextColor(globalDC, color);
    SetBkMode(globalDC, TRANSPARENT);
    
    HFONT font = CreateFontA(fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
                             DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "MS Gothic");
    HFONT oldFont = (HFONT)SelectObject(globalDC, font);
    
    RECT rect = { x, y, x + 800, y + 1000 }; 
    
    DrawTextA(globalDC, text, -1, &rect, DT_LEFT | DT_WORDBREAK | DT_NOCLIP);
    
    SelectObject(globalDC, oldFont);
    DeleteObject(font);
}

void DrawFPS(int x, int y) {
    if (lastFrameTime <= 0.0f) return;
    int fps = (int)(1000.0f / lastFrameTime);
    char buffer[32];
    wsprintfA(buffer, "FPS: %d", fps);
    FluDrawText(buffer, x, y, 20, GREEN);
}
#endif

#ifndef FL_NO_TEXTURES
// Textures
Texture2D LoadTexture(const char* path) {
    Texture2D tex = {0};

    // Check extension
    int isBmp = strstr(path, ".bmp") || strstr(path, ".BMP");

    if (isBmp) {
        tex.hbm = (HBITMAP)LoadImageA(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (!tex.hbm) return tex;
        BITMAP bm;
        GetObject(tex.hbm, sizeof(BITMAP), &bm);
        tex.width  = bm.bmWidth;
        tex.height = bm.bmHeight;
        tex.format = 0;
    } else {
        int channels;
        unsigned char* data = stbi_load(path, &tex.width, &tex.height, &channels, 4);
        
        if (!data) return tex;

        for (int i = 0; i < tex.width * tex.height; i++) {
            unsigned char r = data[i*4+0];
            unsigned char g = data[i*4+1];
            unsigned char b = data[i*4+2];
            unsigned char a = data[i*4+3];
            data[i*4+0] = (unsigned char)(b * a / 255);
            data[i*4+1] = (unsigned char)(g * a / 255);
            data[i*4+2] = (unsigned char)(r * a / 255);
            data[i*4+3] = a;
        }

        BITMAPINFOHEADER bih = {0};
        bih.biSize        = sizeof(BITMAPINFOHEADER);
        bih.biWidth       = tex.width;
        bih.biHeight      = -tex.height;
        bih.biPlanes      = 1;
        bih.biBitCount    = 32;
        bih.biCompression = BI_RGB;
        bih.biSizeImage   = tex.width * tex.height * 4;

        void* bits;
        HDC dc = globalDC ? globalDC : GetDC(NULL);
        tex.hbm = CreateDIBSection(dc, (BITMAPINFO*)&bih, DIB_RGB_COLORS, &bits, NULL, 0);
        if (!tex.hbm) {
            MessageBoxA(NULL, "CreateDIBSection failed", "Debug", MB_OK);
            stbi_image_free(data);
            return tex;
        }
        memcpy(bits, data, tex.width * tex.height * 4);
        stbi_image_free(data);
        tex.format = 1;
    }

    tex.hdc = CreateCompatibleDC(globalDC ? globalDC : GetDC(NULL));
    SelectObject(tex.hdc, tex.hbm);
    return tex;
}

void DrawTexture(Texture2D tex, int x, int y, COLORREF tint) {
    if (!tex.hbm) return;
    if (tex.format) {
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        
        AlphaBlend(globalDC, x, y, tex.width, tex.height,
                   tex.hdc, 0, 0, tex.width, tex.height, bf);
    } else {
        BitBlt(globalDC, x, y, tex.width, tex.height, tex.hdc, 0, 0, SRCCOPY);
    }
}

void DrawTextureEx(Texture2D tex, int x, int y, int width, int height, COLORREF tint) {
    if (!tex.hbm) return;
    StretchBlt(globalDC, x, y, width, height, tex.hdc, 0, 0, tex.width, tex.height, SRCCOPY);
}

void DrawTextureRec(Texture2D tex, FluRectangle src, int x, int y, COLORREF tint) {
    if (!tex.hbm) return;
    BitBlt(globalDC, x, y, (int)src.width, (int)src.height,
           tex.hdc, (int)src.x, (int)src.y, SRCCOPY);
}

void UnloadTexture(Texture2D tex) {
    if (tex.hdc) DeleteDC(tex.hdc);
    if (tex.hbm) DeleteObject(tex.hbm);
}
#endif

bool IsKeyDown(int key) {
    return currKeys[key];
}

bool IsKeyPressed(int key) {
    return currKeys[key] && !prevKeys[key];
}

#define KEY_0 0x30
#define KEY_1 0x31
#define KEY_2 0x32
#define KEY_3 0x33
#define KEY_4 0x34
#define KEY_5 0x35
#define KEY_6 0x36
#define KEY_7 0x37
#define KEY_8 0x38
#define KEY_9 0x39

#define KEY_A 0x41
#define KEY_B 0x42
#define KEY_C 0x43
#define KEY_D 0x44
#define KEY_E 0x45
#define KEY_F 0x46
#define KEY_G 0x47
#define KEY_H 0x48
#define KEY_I 0x49
#define KEY_J 0x4A
#define KEY_K 0x4B
#define KEY_L 0x4C
#define KEY_M 0x4D
#define KEY_N 0x4E
#define KEY_O 0x4F
#define KEY_P 0x50
#define KEY_Q 0x51
#define KEY_R 0x52
#define KEY_S 0x53
#define KEY_T 0x54
#define KEY_U 0x55
#define KEY_V 0x56
#define KEY_W 0x57
#define KEY_X 0x58
#define KEY_Y 0x59
#define KEY_Z 0x5A

#define KEY_SPACE  VK_SPACE
#define KEY_ENTER  VK_RETURN
#define KEY_ESCAPE VK_ESCAPE
#define KEY_BACK   VK_BACK
#define KEY_TAB    VK_TAB
#define KEY_LSHIFT VK_LSHIFT
#define KEY_RSHIFT VK_RSHIFT
#define KEY_LCTRL  VK_LCONTROL
#define KEY_RCTRL  VK_RCONTROL
#define KEY_LALT   VK_LMENU
#define KEY_RALT   VK_RMENU


#define KEY_LEFT VK_LEFT
#define KEY_RIGHT VK_RIGHT
#define KEY_UP VK_UP
#define KEY_DOWN VK_DOWN

// Vector 2 Coll
#ifndef FL_NO_2D
bool CheckCollisionRecs(Rectangle r1, Rectangle r2) {
    return (r1.x < r2.x + r2.width  &&
            r1.x + r1.width > r2.x  &&
            r1.y < r2.y + r2.height &&
            r1.y + r1.height > r2.y);
}
#endif

// Vector3 Coll
#ifndef FL_NO_3D
bool CheckCollisionBoxes(BoundingBox a, BoundingBox b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

BoundingBox GetCubeBoundingBox(Vector3 center, float width, float height, float depth) {
    return (BoundingBox){
        .min = {center.x - width/2,  center.y - height/2, center.z - depth/2},
        .max = {center.x + width/2,  center.y + height/2, center.z + depth/2}
    };
}

// Camera Functions
Vector3 ApplyCamera(Vector3 p, Camera cam) {
    p.x -= cam.position.x;
    p.y -= cam.position.y;
    p.z -= cam.position.z;
    return p;
}

Vector2 Project3D(Vector3 p, Camera cam) {
    Vector3 forward = {
        cam.target.x - cam.position.x,
        cam.target.y - cam.position.y,
        cam.target.z - cam.position.z
    };
    
    float flen = sqrtf(forward.x*forward.x + forward.y*forward.y + forward.z*forward.z);
    forward.x /= flen; forward.y /= flen; forward.z /= flen;
    
    Vector3 right = {
        forward.y*cam.up.z - forward.z*cam.up.y,
        forward.z*cam.up.x - forward.x*cam.up.z,
        forward.x*cam.up.y - forward.y*cam.up.x
    };
    float rlen = sqrtf(right.x*right.x + right.y*right.y + right.z*right.z);
    right.x /= rlen; right.y /= rlen; right.z /= rlen;
    
    Vector3 up = {
        right.y*(-forward.z) - right.z*(-forward.y),
        right.z*(-forward.x) - right.x*(-forward.z),
        right.x*(-forward.y) - right.y*(-forward.x)
    };
    
    Vector3 rel = {
        p.x - cam.position.x,
        p.y - cam.position.y,
        p.z - cam.position.z
    };

    // Project
    float ex =  right.x*rel.x   + right.y*rel.y   + right.z*rel.z;
    float ey =  up.x*rel.x      + up.y*rel.y      + up.z*rel.z;
    float ez =  forward.x*rel.x + forward.y*rel.y + forward.z*rel.z;

    if (ez <= 0.1f) ez = 0.1f;

    float fovRad = cam.fovy * (3.14159f / 180.0f);
    float scale  = 1.0f / tanf(fovRad / 2.0f);

    Vector2 result;
    result.x = (ex * scale) / ez * (clientRect.right  / 2) + (clientRect.right  / 2);
    result.y = -(ey * scale) / ez * (clientRect.bottom / 2) + (clientRect.bottom / 2);
    return result;
}

void DrawLine3D(Vector3 a, Vector3 b, COLORREF color) {
    Vector2 p1 = Project3D(a, currentCamera);
    Vector2 p2 = Project3D(b, currentCamera);
    SelectObject(globalDC, GetStockObject(DC_PEN));
    SetDCPenColor(globalDC, color);
    MoveToEx(globalDC, (int)p1.x, (int)p1.y, NULL);
    LineTo(globalDC, (int)p2.x, (int)p2.y);
}

void DrawCubeWires(Vector3 center, float width, float height, float depth, COLORREF color) {
    float hx = width  / 2;
    float hy = height / 2;
    float hz = depth  / 2;

    // 8 corners
    Vector3 v[8] = {
        {center.x - hx, center.y - hy, center.z - hz}, // 0 left  bottom front
        {center.x + hx, center.y - hy, center.z - hz}, // 1 right bottom front
        {center.x + hx, center.y + hy, center.z - hz}, // 2 right top    front
        {center.x - hx, center.y + hy, center.z - hz}, // 3 left  top    front
        {center.x - hx, center.y - hy, center.z + hz}, // 4 left  bottom back
        {center.x + hx, center.y - hy, center.z + hz}, // 5 right bottom back
        {center.x + hx, center.y + hy, center.z + hz}, // 6 right top    back
        {center.x - hx, center.y + hy, center.z + hz}, // 7 left  top    back
    };

    // front face
    DrawLine3D(v[0], v[1], color);
    DrawLine3D(v[1], v[2], color);
    DrawLine3D(v[2], v[3], color);
    DrawLine3D(v[3], v[0], color);
    // back face
    DrawLine3D(v[4], v[5], color);
    DrawLine3D(v[5], v[6], color);
    DrawLine3D(v[6], v[7], color);
    DrawLine3D(v[7], v[4], color);
    // connectors
    DrawLine3D(v[0], v[4], color);
    DrawLine3D(v[1], v[5], color);
    DrawLine3D(v[2], v[6], color);
    DrawLine3D(v[3], v[7], color);
}

int IsFacingCamera(Vector3 a, Vector3 b, Vector3 c, Camera cam) {
    Vector3 ab = {b.x-a.x, b.y-a.y, b.z-a.z};
    Vector3 ac = {c.x-a.x, c.y-a.y, c.z-a.z};
    
    Vector3 normal = {
        ab.y*ac.z - ab.z*ac.y,
        ab.z*ac.x - ab.x*ac.z,
        ab.x*ac.y - ab.y*ac.x
    };
    
    Vector3 toCamera = {
        cam.position.x - a.x,
        cam.position.y - a.y,
        cam.position.z - a.z
    };
    
    float dot = normal.x*toCamera.x + normal.y*toCamera.y + normal.z*toCamera.z;
    return dot > 0.0f;
}

// Tri's
#ifndef FL_NO_TRI
typedef struct {
    Vector3 a, b, c;
    COLORREF color;
    float depth;
} Triangle3D;

#define MAX_TRIANGLES 4096
Triangle3D triangleBuffer[MAX_TRIANGLES];
int triangleCount = 0;

void PushTriangle3D(Vector3 a, Vector3 b, Vector3 c, COLORREF color) {
    if (triangleCount >= MAX_TRIANGLES) return;
    if (!IsFacingCamera(a, b, c, currentCamera)) return;

    float depth = ((a.x - currentCamera.position.x) * (a.x - currentCamera.position.x) +
                   (a.y - currentCamera.position.y) * (a.y - currentCamera.position.y) +
                   (a.z - currentCamera.position.z) * (a.z - currentCamera.position.z) +
                   (b.x - currentCamera.position.x) * (b.x - currentCamera.position.x) +
                   (b.y - currentCamera.position.y) * (b.y - currentCamera.position.y) +
                   (b.z - currentCamera.position.z) * (b.z - currentCamera.position.z) +
                   (c.x - currentCamera.position.x) * (c.x - currentCamera.position.x) +
                   (c.y - currentCamera.position.y) * (c.y - currentCamera.position.y) +
                   (c.z - currentCamera.position.z) * (c.z - currentCamera.position.z)) / 3.0f;

    triangleBuffer[triangleCount++] = (Triangle3D){a, b, c, color, depth};
}

int CompareTriangles(const void* x, const void* y) {
    float da = ((Triangle3D*)x)->depth;
    float db = ((Triangle3D*)y)->depth;
    return (da < db) - (da > db); // back to front
}

void FlushTriangles() {
    qsort(triangleBuffer, triangleCount, sizeof(Triangle3D), CompareTriangles);
    for (int i = 0; i < triangleCount; i++) {
        Triangle3D* t = &triangleBuffer[i];
        Vector2 p1 = Project3D(t->a, currentCamera);
        Vector2 p2 = Project3D(t->b, currentCamera);
        Vector2 p3 = Project3D(t->c, currentCamera);
        POINT pts[3] = {
            {(int)p1.x, (int)p1.y},
            {(int)p2.x, (int)p2.y},
            {(int)p3.x, (int)p3.y},
        };
        SetDCBrushColor(globalDC, t->color);
        SetDCPenColor(globalDC, t->color);
        Polygon(globalDC, pts, 3);
    }
    triangleCount = 0;
}

void DrawTriangle3D(Vector3 a, Vector3 b, Vector3 c, COLORREF color) {
    PushTriangle3D(a, b, c, color);
}

void DrawCube(Vector3 center, float width, float height, float depth, COLORREF color) {
    float hx = width  / 2;
    float hy = height / 2;
    float hz = depth  / 2;
    
    Vector3 v[8] = {
        {center.x - hx, center.y - hy, center.z - hz},
        {center.x + hx, center.y - hy, center.z - hz},
        {center.x + hx, center.y + hy, center.z - hz},
        {center.x - hx, center.y + hy, center.z - hz},
        {center.x - hx, center.y - hy, center.z + hz},
        {center.x + hx, center.y - hy, center.z + hz},
        {center.x + hx, center.y + hy, center.z + hz},
        {center.x - hx, center.y + hy, center.z + hz},
    };
    
    // front
    DrawTriangle3D(v[0], v[1], v[2], color);
    DrawTriangle3D(v[0], v[2], v[3], color);
    // back
    DrawTriangle3D(v[5], v[4], v[7], color);
    DrawTriangle3D(v[5], v[7], v[6], color);
    // left
    DrawTriangle3D(v[4], v[0], v[3], color);
    DrawTriangle3D(v[4], v[3], v[7], color);
    // right
    DrawTriangle3D(v[1], v[5], v[6], color);
    DrawTriangle3D(v[1], v[6], v[2], color);
    // top
    DrawTriangle3D(v[3], v[2], v[6], color);
    DrawTriangle3D(v[3], v[6], v[7], color);
    // bottom
    DrawTriangle3D(v[4], v[5], v[1], color);
    DrawTriangle3D(v[4], v[1], v[0], color);
}
#endif

// Camera Rotations
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

void BeginMode3D(Camera cam) {
    currentCamera = cam;
}

void EndMode3D() {
    #ifndef FL_NO_TRI
        FlushTriangles();
    #endif
}
#endif