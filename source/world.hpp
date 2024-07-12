#pragma once

#include <vector>

#include "chunk.hpp"
#include "camera.hpp"

const s32 RENDER_DISTANCE = 4;
const s32 TRACK_DISTANCE = RENDER_DISTANCE + 2;
const s32 TRACK_GRID_SIZE = TRACK_DISTANCE * 2 + 1;

class World {
public:
    World(const Camera& camera_, int uPosition_);
    ~World() = default;

    void render();

    Block getBlock(s32 x, s32 y, s32 z) {
        if (y < 0 || y >= CHUNK_HEIGHT) {
            return Block(BlockType::None);
        }

        s32 chunkX = std::floor((float)x / (float)CHUNK_WIDTH);
        s32 chunkZ = std::floor((float)z / (float)CHUNK_WIDTH);
        s32 chunkRelX = chunkX - cameraChunkX;
        s32 chunkRelZ = chunkZ - cameraChunkZ;
        if (std::abs(chunkRelX) > TRACK_DISTANCE || std::abs(chunkRelZ) > TRACK_DISTANCE) {
            return Block(BlockType::None);
        }

        Chunk* chunk = getTrackedChunk(chunkRelX, chunkRelZ);
        if (!chunk) {
            return Block(BlockType::None);
        }
        //std::cout << "B2: " << chunkX << ", " << chunkZ << " : " << x - chunkX * CHUNK_WIDTH << ", " << z - chunkZ * CHUNK_WIDTH << std::endl;
        //std::cout << "B3: " << chunk->getX() << ", " << chunk->getZ() << std::endl;

        //return Block(BlockType::Dirt);
        return chunk->getBlock(x - chunkX * CHUNK_WIDTH, y, z - chunkZ * CHUNK_WIDTH);
    }

    const Camera& getCamera() const {
        return camera;
    }

private:
    const Camera& camera;
    int uPosition;

    s32 cameraChunkX = 0;
    s32 cameraChunkZ = 0;

    std::vector<Chunk> chunks;
    Chunk* trackedChunks[TRACK_GRID_SIZE][TRACK_GRID_SIZE];

    //u32 activeThreads = 0;
    //std::vector<Chunk*> generationQueue;

    Chunk*& getTrackedChunk(s32 chunkRelX, s32 chunkRelZ) {
        // HACK: just in case
        if (chunkRelX > TRACK_DISTANCE || chunkRelX < -TRACK_DISTANCE || chunkRelZ > TRACK_DISTANCE || chunkRelZ < -TRACK_DISTANCE) {
            std::cout << "error: " << chunkRelX << ", " << chunkRelZ << std::endl;
        }

        return trackedChunks[chunkRelX + TRACK_DISTANCE][chunkRelZ + TRACK_DISTANCE];
    }

    void findTrackedChunks();

    //void updateQueues();
};
