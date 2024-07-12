#include "aabb.hpp"

bool AABB::collidesWith(const AABB& other) {
    return position.x - scale.x / 2.0f < other.position.x + other.scale.x / 2.0f && position.x + scale.x / 2.0f > other.position.x - other.scale.x / 2.0f &&
           position.y - scale.y / 2.0f < other.position.y + other.scale.y / 2.0f && position.y + scale.y / 2.0f > other.position.y - other.scale.y / 2.0f &&
           position.z - scale.z / 2.0f < other.position.z + other.scale.z / 2.0f && position.z + scale.z / 2.0f > other.position.z - other.scale.z / 2.0f;
}

//#include <iostream>

bool AABB::isInFrustum(const Frustum& frustum) {
    return (isInOrInFrontOfPlan(frustum, frustum.leftFace) &&
            isInOrInFrontOfPlan(frustum, frustum.rightFace) &&
            isInOrInFrontOfPlan(frustum, frustum.topFace) &&
            isInOrInFrontOfPlan(frustum, frustum.bottomFace) &&
            isInOrInFrontOfPlan(frustum, frustum.nearFace) &&
            isInOrInFrontOfPlan(frustum, frustum.farFace));
}

bool AABB::isInOrInFrontOfPlan(const Frustum& frustum, const Plan& face) {
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    const float r = scale.x * 0.5f * std::abs(face.normal.x) + scale.y * 0.5f * std::abs(face.normal.y) + scale.z * 0.5f * std::abs(face.normal.z);
    //std::cout << -r << " : " << face.getSignedDistanceToPlan(position) << std::endl << std::endl;

    return -r <= face.getSignedDistanceToPlan(position);
}
