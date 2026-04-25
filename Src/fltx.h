// FLTX 1.0
// Flulib 1.1
// Under Z-Lib

// Textured Tri's
typedef struct {
    Vector3 a, b, c;
    Texture2D tex;
    float u1,v1, u2,v2, u3,v3;
    float depth;
} TexturedTriangle3D;

#define MAX_TEX_TRIANGLES 4096
TexturedTriangle3D texTriangleBuffer[MAX_TEX_TRIANGLES];
int texTriangleCount = 0;

void PushTexturedTriangle3D(Vector3 a, Vector3 b, Vector3 c, Texture2D tex, float u1, float v1, float u2, float v2, float u3, float v3) {
    if (texTriangleCount >= MAX_TEX_TRIANGLES) return;
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
    
    texTriangleBuffer[texTriangleCount++] = (TexturedTriangle3D){a, b, c, tex, u1,v1, u2,v2, u3,v3, depth};
}

void FlushTexturedTriangles() {
    for (int i = 0; i < texTriangleCount; i++) {
        TexturedTriangle3D* t = &texTriangleBuffer[i];
        Vector2 p1 = Project3D(t->a, currentCamera);
        Vector2 p2 = Project3D(t->b, currentCamera);
        Vector2 p3 = Project3D(t->c, currentCamera);
        
        // Get bounding box
        int minX = (int)fminf(p1.x, fminf(p2.x, p3.x));
        int minY = (int)fminf(p1.y, fminf(p2.y, p3.y));
        int maxX = (int)fmaxf(p1.x, fmaxf(p2.x, p3.x));
        int maxY = (int)fmaxf(p1.y, fmaxf(p2.y, p3.y));
        
        float denom = (p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y);
        if (fabsf(denom) < 0.001f) continue;
        
        for (int py = minY; py <= maxY; py++) {
            for (int px = minX; px <= maxX; px++) {
                float w1 = ((p2.y - p3.y) * (px - p3.x) + (p3.x - p2.x) * (py - p3.y)) / denom;
                float w2 = ((p3.y - p1.y) * (px - p3.x) + (p1.x - p3.x) * (py - p3.y)) / denom;
                float w3 = 1.0f - w1 - w2;
                
                if (w1 < 0 || w2 < 0 || w3 < 0) continue;
                
                // Interpolate UV
                float u = w1 * t->u1 + w2 * t->u2 + w3 * t->u3;
                float v = w1 * t->v1 + w2 * t->v2 + w3 * t->v3;
                
                int tx = (int)(u * (t->tex.width  - 1));
                int ty = (int)(v * (t->tex.height - 1));
                tx = tx < 0 ? 0 : tx >= t->tex.width  ? t->tex.width  - 1 : tx;
                ty = ty < 0 ? 0 : ty >= t->tex.height ? t->tex.height - 1 : ty;

                COLORREF c = GetPixel(t->tex.hdc, tx, ty);
                SetPixel(globalDC, px, py, c);
            }
        }
    }
    texTriangleCount = 0;
}

void DrawCubeTextured(Vector3 center, float width, float height, float depth, Texture2D tex) {
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
    PushTexturedTriangle3D(v[0],v[1],v[2], tex, 0,1, 1,1, 1,0);
    PushTexturedTriangle3D(v[0],v[2],v[3], tex, 0,1, 1,0, 0,0);
    // back
    PushTexturedTriangle3D(v[5],v[4],v[7], tex, 0,1, 1,1, 1,0);
    PushTexturedTriangle3D(v[5],v[7],v[6], tex, 0,1, 1,0, 0,0);
    // left
    PushTexturedTriangle3D(v[4],v[0],v[3], tex, 0,1, 1,1, 1,0);
    PushTexturedTriangle3D(v[4],v[3],v[7], tex, 0,1, 1,0, 0,0);
    // right
    PushTexturedTriangle3D(v[1],v[5],v[6], tex, 0,1, 1,1, 1,0);
    PushTexturedTriangle3D(v[1],v[6],v[2], tex, 0,1, 1,0, 0,0);
    // top
    PushTexturedTriangle3D(v[3],v[2],v[6], tex, 0,1, 1,1, 1,0);
    PushTexturedTriangle3D(v[3],v[6],v[7], tex, 0,1, 1,0, 0,0);
    // bottom
    PushTexturedTriangle3D(v[4],v[5],v[1], tex, 0,1, 1,1, 1,0);
    PushTexturedTriangle3D(v[4],v[1],v[0], tex, 0,1, 1,0, 0,0);
}