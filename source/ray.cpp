#include "ray.hpp"

float Ray::intersectsCube(C3D_FVec cubePos) {
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    C3D_FVec invDir = C3D_FVec{ .z = 1.0f / direction.z, .y = 1.0f / direction.y, .x = 1.0f / direction.x };

    int sign[3];
    sign[0] = (invDir.x < 0);
    sign[1] = (invDir.y < 0);
    sign[2] = (invDir.z < 0);

    C3D_FVec bounds[2];
    bounds[0] = FVec3_Subtract(cubePos, C3D_FVec{ .z = 0.5f, .y = 0.5f, .x = 0.5f });
    bounds[1] = FVec3_Add(cubePos, C3D_FVec{ .z = 0.5f, .y = 0.5f, .x = 0.5f });

    tmin = (bounds[sign[0]].x - origin.x) * invDir.x;
    tmax = (bounds[1 - sign[0]].x - origin.x) * invDir.x;
    tymin = (bounds[sign[1]].y - origin.y) * invDir.y;
    tymax = (bounds[1 - sign[1]].y - origin.y) * invDir.y;

    if ((tmin > tymax) || (tymin > tmax))
        return -1.0f;

    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    tzmin = (bounds[sign[2]].z - origin.z) * invDir.z;
    tzmax = (bounds[1 - sign[2]].z - origin.z) * invDir.z;

    if ((tmin > tzmax) || (tzmin > tmax))
        return -1.0f;

    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    return tmin;
}
