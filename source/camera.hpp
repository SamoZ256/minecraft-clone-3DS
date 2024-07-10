#pragma once

class Camera {
public:
    C3D_FVec position{};
    C3D_FVec direction{.z = 1.0f};
    C3D_FVec up{.y = 1.0f};

    Camera() = default;
    ~Camera() = default;
};
