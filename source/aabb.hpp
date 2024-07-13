#pragma once

#include "frustum.hpp"

class AABB {
public:
    C3D_FVec position = float3(0.0f);
    C3D_FVec scale = float3(1.0f);

    bool collidesWith(const AABB& other);

    bool isInFrustum(const Frustum& frustum);

    bool isInOrInFrontOfPlan(const Frustum& frustum, const Plan& face);
};
