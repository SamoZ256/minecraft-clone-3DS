#include "world.hpp"

World::World(const Camera& camera_, int uPosition_) : camera{camera_}, uPosition{uPosition_} {
    chunks.reserve(100);

    for (s16 z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; z++) {
        for (s16 x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; x++) {
            chunks.emplace_back(*this, uPosition, x, z);
        }
    }
}

void World::render() {
    findTrackedChunks();

    for (s16 z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; z++) {
        for (s16 x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; x++) {
            Chunk* chunk = getTrackedChunk(x, z);
            if (chunk) {
                chunk->render();
            }
        }
    }
}

void World::findTrackedChunks() {
    cameraChunkX = camera.position.x / CHUNK_WIDTH;
    cameraChunkZ = camera.position.z / CHUNK_WIDTH;

    for (u8 z = 0; z < TRACK_GRID_SIZE; z++) {
        for (u8 x = 0; x < TRACK_GRID_SIZE; x++) {
            trackedChunks[z][x] = nullptr;
        }
    }

    for (auto& chunk : chunks) {
        s32 chunkRelX = chunk.getX() - cameraChunkX;
        s32 chunkRelZ = chunk.getZ() - cameraChunkZ;
        if (std::abs(chunkRelX) <= TRACK_DISTANCE && std::abs(chunkRelZ) <= TRACK_DISTANCE) {
            // TODO: check if in the circle
            trackedChunks[chunkRelZ + TRACK_DISTANCE][chunkRelX + TRACK_DISTANCE] = &chunk;
        }
    }

    // TODO: iterate over trackedChunks and generate new ones
}
