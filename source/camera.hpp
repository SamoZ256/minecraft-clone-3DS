#pragma once

class Camera {
public:
    C3D_FVec position{0.0f, 0.0f, 0.0f};
    C3D_FVec direction{0.0f, 0.0f, 1.0f};
    C3D_FVec up{0.0f, 1.0f, 0.0f};

    Camera() = default;
    ~Camera() = default;
};
