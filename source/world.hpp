#pragma once

#include <vector>

#include "chunk.hpp"
#include "camera.hpp"

const s16 RENDER_DISTANCE = 1;

class World {
public:
    World(int uPosition_);
    ~World() = default;

    void render();

private:
    std::vector<Chunk> chunks;

    int uPosition;
};
