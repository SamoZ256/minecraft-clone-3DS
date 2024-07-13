#pragma once

#include "common.hpp"

class Ray {
public:
    C3D_FVec origin;
    C3D_FVec direction;

    Ray(C3D_FVec aOrigin, C3D_FVec aDirection) : origin(aOrigin), direction(aDirection) {}

    float intersectsCube(C3D_FVec cubePos);
};
