#include "frustum.hpp"

float Plan::getSignedDistanceToPlan(const C3D_FVec& point) const {
    return FVec3_Dot(normal, point) - distance;
}

void Frustum::create(const C3D_FVec& position, const C3D_FVec& direction, float nearPlane, float farPlane, float fov, float aspectRatio) {
    // TODO: uncomment
    /*
    const float halfVSide = farPlane * tanf(fov * .5f);
    const float halfHSide = halfVSide * aspectRatio;
    const C3D_FVec3 frontMultFar = farPlane * direction;

    C3D_FVec3 right = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
    C3D_FVec3 up    = glm::normalize(glm::cross(right, direction));

    nearFace = Plan(position + nearPlane * direction, direction);
    farFace = Plan(position + frontMultFar, -direction);
    rightFace = Plan(position, FVec_Cross(up, frontMultFar + right * halfHSide));
    leftFace = Plan(position, FVec_Cross(frontMultFar - right * halfHSide, up));
    topFace = Plan(position, FVec_Cross(right, frontMultFar - up * halfVSide));
    bottomFace = Plan(position, FVec_Cross(frontMultFar + up * halfVSide, right));
    */
}
