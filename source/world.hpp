#pragma once

#include <vector>

#include "chunk.hpp"
#include "camera.hpp"

const s32 RENDER_DISTANCE = 5;
const s32 TRACK_DISTANCE = RENDER_DISTANCE + 2;
const s32 TRACK_GRID_SIZE = TRACK_DISTANCE * 2 + 1;

const u8 BLOCK_BREAK_DIST = 8;

struct Intersection {
    bool found = false;
    s32 x = 0, y = 0, z = 0;
    s8 placeNormal[3] = {0, 0, 0};
};

class World {
public:
    double temperatureNoiseSeed;
    double humidityNoiseSeed;
    double noiseSeed;

    World(Camera& camera_, int uPosition_);
    ~World() = default;

    void render();

    void moveCamera(const C3D_FVec& movement, bool& isOnGround, bool& wallJump);

    Intersection getIntersection();

    void breakBlock(const Intersection& intersection);

    void placeBlock(const Intersection& intersection, BlockType ty);

    // TODO: make these 2 functions share the code
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

        return chunk->getBlock(x - chunkX * CHUNK_WIDTH, y, z - chunkZ * CHUNK_WIDTH);
    }

    void setBlockType(s32 x, s32 y, s32 z, BlockType ty) {
        if (y < 0 || y >= CHUNK_HEIGHT) {
            return;
        }

        s32 chunkX = std::floor((float)x / (float)CHUNK_WIDTH);
        s32 chunkZ = std::floor((float)z / (float)CHUNK_WIDTH);
        s32 chunkRelX = chunkX - cameraChunkX;
        s32 chunkRelZ = chunkZ - cameraChunkZ;
        if (std::abs(chunkRelX) > TRACK_DISTANCE || std::abs(chunkRelZ) > TRACK_DISTANCE) {
            return;
        }

        Chunk* chunk = getTrackedChunk(chunkRelX, chunkRelZ);
        if (!chunk) {
            return;
        }

        return chunk->setBlockType(x - chunkX * CHUNK_WIDTH, y, z - chunkZ * CHUNK_WIDTH, ty);
    }

    const Camera& getCamera() const {
        return camera;
    }

private:
    Camera& camera;
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

    void updateChunksAfterBlockChange(s32 x, s32 y, s32 z, bool isPlace);
};
