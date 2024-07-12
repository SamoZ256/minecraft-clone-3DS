#pragma once

#include "frustum.hpp"

class AABB {
public:
    C3D_FVec position{.z = 0.0f, .y = 0.0f, .x = 0.0f};
    C3D_FVec scale = C3D_FVec{.z = 1.0f, .y = 1.0f, .x = 1.0f};

    bool collidesWith(const AABB& other);

    bool isInFrustum(const Frustum& frustum);

    bool isInOrInFrontOfPlan(const Frustum& frustum, const Plan& face);
};
