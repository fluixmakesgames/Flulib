// FLMath 1.0
// Flulib 1.1
// Under Z-Lib

Vector3 Vector3Add(Vector3 a, Vector3 b) {
    return (Vector3){a.x+b.x, a.y+b.y, a.z+b.z};
}

Vector3 Vector3Subtract(Vector3 a, Vector3 b) {
    return (Vector3){a.x-b.x, a.y-b.y, a.z-b.z};
}

Vector3 Vector3Scale(Vector3 v, float s) {
    return (Vector3){v.x*s, v.y*s, v.z*s};
}

float Vector3Length(Vector3 v) {
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

Vector3 Vector3Normalize(Vector3 v) {
    float len = Vector3Length(v);
    if (len == 0) return (Vector3){0,0,0};
    return (Vector3){v.x/len, v.y/len, v.z/len};
}

float Vector3DotProduct(Vector3 a, Vector3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector3 Vector3CrossProduct(Vector3 a, Vector3 b) {
    return (Vector3){
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

Vector3 Vector3Lerp(Vector3 a, Vector3 b, float t) {
    return (Vector3){
        a.x + (b.x-a.x)*t,
        a.y + (b.y-a.y)*t,
        a.z + (b.z-a.z)*t
    };
}