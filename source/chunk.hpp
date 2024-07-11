#pragma once

#include "block.hpp"
#include "vertex.hpp"

const u8 CHUNK_WIDTH = 16;
const u8 CHUNK_HEIGHT = 64;

struct VboData {
    void* vertexData;
    void* indexData;
    size_t indexCount;

    VboData() = default;
    VboData(const std::vector<Vertex>& vertices, const std::vector<u16>& indices) {
        indexCount = indices.size();

        // Vertices
        vertexData = linearAlloc(vertices.size() * sizeof(Vertex));
        memcpy(vertexData, vertices.data(), vertices.size() * sizeof(Vertex));

        // Indices
        indexData = linearAlloc(indices.size() * sizeof(u16));
        memcpy(indexData, indices.data(), indices.size() * sizeof(u16));
    }

    void freeData() {
        linearFree(vertexData);
        linearFree(indexData);
    }
};

class World;

class Chunk {
public:
    Chunk(World& world_, int uPosition_, s32 x_, s32 z_);
    ~Chunk() = default;

    void render();

    void freeData();

    Block getBlock(s32 blockX, s32 blockY, s32 blockZ) const {
        return blocks[blockX][blockY][blockZ];
    }

    void setBlock(s32 blockX, s32 blockY, s32 blockZ, Block block) {
        blocks[blockX][blockY][blockZ] = block;
    }

    void setBlockChecked(s32 blockX, s32 blockY, s32 blockZ, Block block) {
        if (blockX >= 0 && blockX < CHUNK_WIDTH &&
            blockY >= 0 && blockY < CHUNK_HEIGHT &&
            blockZ >= 0 && blockZ < CHUNK_WIDTH) {
            setBlock(blockX, blockY, blockZ, block);
        }
    }

    s32 getX() const {
        return x;
    }

    s32 getZ() const {
        return z;
    }

private:
    World& world;
    int uPosition;
    s32 x, z;

    Block blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_WIDTH];

    bool allocated = false;
    VboData vboData;
    C3D_BufInfo vbo;

    void allocate();
};
