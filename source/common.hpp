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
    T z, y, x;

    PackedVec3(T x_, T y_, T z_) : z{z_}, y{y_}, x{x_} {}
};
