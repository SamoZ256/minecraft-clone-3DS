#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <3ds.h>
#include <tex3ds.h>

#include "math.hpp"

template<typename T>
struct TexCoord {
    // U and V is swapped to match the PICA200 coordinate system
    T v, u;

    TexCoord(T u_, T v_) : v{v_}, u{u_} {}
};
