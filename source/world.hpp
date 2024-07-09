#pragma once

#include <vector>

#include "chunk.hpp"

const s16 RENDER_DISTANCE = 1;

class World {
public:
    World(int uPosition_);
    ~World() = default;

    void render() const;

private:
    std::vector<Chunk> chunks;

    int uPosition;
};
