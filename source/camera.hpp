#pragma once

#include "aabb.hpp"

class Camera {
public:
    AABB aabb{};
    C3D_FVec direction = float3(0.0f, 0.0f, 1.0f);
        C3D_FVec up = float3(0.0f, 1.0f, 0.0f);

    Camera() = default;
    ~Camera() = default;

    C3D_FVec getPosition() const {
        return aabb.position + float3(0.0f, aabb.scale.y * 0.4f, 0.0f);
    }
};
