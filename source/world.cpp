#include "world.hpp"

World::World(int uPosition_) : uPosition{uPosition_} {
    chunks.reserve(100);

    for (s16 z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; z++) {
        for (s16 x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; x++) {
            chunks.emplace_back(x, z, uPosition);
        }
    }
}

void World::render() const {
    for (const auto& chunk : chunks) {
        chunk.render();
    }
}
