#pragma once

#include "aabb.hpp"

class Camera {
public:
    AABB aabb{};
    C3D_FVec direction{.z = 1.0f};
    C3D_FVec up{.y = 1.0f};

    Camera() = default;
    ~Camera() = default;

    C3D_FVec getPosition() const {
        return FVec3_Add(aabb.position, C3D_FVec{.z = 0.0f, .y = aabb.scale.y * 0.4f, .x = 0.0f});
    }
};
