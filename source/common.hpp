#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <3ds.h>
#include <citro3d.h>
#include <tex3ds.h>

template<typename T>
struct PackedVec2 {
    T y, x;

    PackedVec2(T x_, T y_) : y{y_}, x{x_} {}
};

template<typename T>
struct PackedVec3 {
    T x, y, z;

    PackedVec3(T x_, T y_, T z_) : x{x_}, y{y_}, z{z_} {}
};
