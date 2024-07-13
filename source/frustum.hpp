#pragma once

#include "common.hpp"

struct Plan {
    C3D_FVec normal = float3(0.0f, 1.0f, 0.0f);
    float distance = 0.0f;

    Plan() = default;

    Plan(const C3D_FVec& point, const C3D_FVec& aNormal) : normal(FVec3_Normalize(aNormal)), distance(FVec3_Dot(normal, point)) {}

    float getSignedDistanceToPlan(const C3D_FVec& point) const;
};

class Frustum {
public:
    Plan topFace;
    Plan bottomFace;
    Plan farFace;
    Plan nearFace;
    Plan rightFace;
    Plan leftFace;

    Frustum() = default;

    void create(const C3D_FVec& position, const C3D_FVec& direction, float nearPlane, float farPlane, float fov, float aspectRatio);
};
