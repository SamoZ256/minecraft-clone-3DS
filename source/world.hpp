#pragma once

#include <vector>

#include "chunk.hpp"
#include "camera.hpp"

const u8 RENDER_DISTANCE = 2;
const u8 TRACK_DISTANCE = RENDER_DISTANCE + 1;
const u8 TRACK_GRID_SIZE = TRACK_DISTANCE * 2 + 1;

class World {
public:
    World(const Camera& camera_, int uPosition_);
    ~World() = default;

    void render();

    Block getBlock(s32 x, s32 y, s32 z) {
        // TODO: uncomment
        /*
        if (y < 0 || y >= CHUNK_HEIGHT) {
            return Block(BlockType::None);
        }

        s32 chunkX = x / CHUNK_WIDTH;
        s32 chunkZ = z / CHUNK_WIDTH;
        s32 chunkRelX = chunkX - cameraChunkX;
        s32 chunkRelZ = chunkZ - cameraChunkZ;
        const Chunk* chunk = getTrackedChunk(chunkRelX, chunkRelZ);
        if (chunk == nullptr) {
            return Block(BlockType::None);
        }

        return chunk->getBlock(x % CHUNK_WIDTH, y, z % CHUNK_WIDTH);
        */
        return Block(BlockType::None);
    }

private:
    const Camera& camera;
    int uPosition;

    s32 cameraChunkX = 0;
    s32 cameraChunkZ = 0;

    std::vector<Chunk> chunks;
    Chunk* trackedChunks[TRACK_GRID_SIZE][TRACK_GRID_SIZE];

    Chunk*& getTrackedChunk(s32 chunkRelX, s32 chunkRelZ) {
        // TODO: check if out of bounds
        return trackedChunks[chunkRelX + TRACK_DISTANCE][chunkRelZ + TRACK_DISTANCE];
    }

    void findTrackedChunks();
};
