#pragma once

#include <vector>

#include "chunk.hpp"
#include "camera.hpp"

const s32 RENDER_DISTANCE = 5;
const s32 TRACK_DISTANCE = RENDER_DISTANCE + 2;
const s32 TRACK_GRID_SIZE = TRACK_DISTANCE * 2 + 1;
const s32 DEALLOCATE_DISTANCE = TRACK_DISTANCE + 1;

const u8 BLOCK_BREAK_DIST = 8;

struct Intersection {
    bool found = false;
    s32 x = 0, y = 0, z = 0;
    s8 placeNormal[3] = {0, 0, 0};
};

union Int32 {
    s32 s;
    u32 u;
};

struct ModifiedBlock {
    // X and Z are relative to the chunk
    s32 x, y, z;
    BlockType ty;
};

class World {
public:
    double temperatureNoiseSeed;
    double humidityNoiseSeed;
    double noiseSeed;

    World(Camera& camera_, int uPosition_);
    ~World() = default;

    void save(void*& saveData, u32& saveDataSize) const;

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

        s32 relX = x - chunkX * CHUNK_WIDTH;
        s32 relZ = z - chunkZ * CHUNK_WIDTH;
        chunk->setBlockType(relX, y, relZ, ty);

        // Save the modified block
        getModifiedBlocksForChunk(chunkX, chunkZ).push_back({relX, y, relZ, ty});
    }

    const Camera& getCamera() const {
        return camera;
    }

    std::vector<ModifiedBlock>& getModifiedBlocksForChunk(s32 chunkX, s32 chunkZ) {
        return modifiedBlocks[getChunkHash(chunkX, chunkZ)];
    }

private:
    Camera& camera;
    int uPosition;

    u32 seed;

    s32 cameraChunkX = 0;
    s32 cameraChunkZ = 0;

    std::vector<Chunk*> chunks;
    Chunk* trackedChunks[TRACK_GRID_SIZE][TRACK_GRID_SIZE];
    std::vector<u32> freeChunkIndices;

    std::map<u64, std::vector<ModifiedBlock>> modifiedBlocks;

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

    u64 getChunkHash(s32 x, s32 z) {
        return (u64)Int32{ .s = x }.u << 32 | (u64)Int32{ .s = z }.u;
    }
};
