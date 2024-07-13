#pragma once

#include <citro3d.h>

inline C3D_FVec float3(float x, float y, float z) {
    return C3D_FVec{ .z = z, .y = y, .x = x };
}

inline C3D_FVec float3(float v) {
    return C3D_FVec{ .z = v, .y = v, .x = v };
}

inline C3D_FVec operator+(const C3D_FVec& l, const C3D_FVec& r) {
    return FVec3_Add(l, r);
}

inline C3D_FVec operator+(const C3D_FVec& l, float r) {
    return C3D_FVec{ .z = l.z + r, .y = l.y + r, .x = l.x + r };
}

inline C3D_FVec operator-(const C3D_FVec& l, const C3D_FVec& r) {
    return FVec3_Subtract(l, r);
}

inline C3D_FVec operator-(const C3D_FVec& l, float r) {
    return C3D_FVec{ .z = l.z - r, .y = l.y - r, .x = l.x - r };
}

inline C3D_FVec operator*(const C3D_FVec& l, const C3D_FVec& r) {
    return C3D_FVec{ .z = l.z * r.z, .y = l.y * r.y, .x = l.x * r.x };
}

inline C3D_FVec operator*(const C3D_FVec& l, float r) {
    return C3D_FVec{ .z = l.z * r, .y = l.y * r, .x = l.x * r };
}

inline C3D_FVec operator*(float l, const C3D_FVec& r) {
    return C3D_FVec{ .z = l * r.z, .y = l * r.y, .x = l * r.x };
}

inline C3D_FVec operator/(const C3D_FVec& l, const C3D_FVec& r) {
    return C3D_FVec{ .z = l.z / r.z, .y = l.y / r.y, .x = l.x / r.x };
}

inline C3D_FVec operator/(const C3D_FVec& l, float r) {
    return C3D_FVec{ .z = l.z / r, .y = l.y / r, .x = l.x / r };
}

inline C3D_FVec operator/(float l, const C3D_FVec& r) {
    return C3D_FVec{ .z = l / r.z, .y = l / r.y, .x = l / r.x };
}
