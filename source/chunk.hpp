#pragma once

#include "block.hpp"
#include "vertex.hpp"

const u8 CHUNK_WIDTH = 8;
const u8 CHUNK_HEIGHT = 32;

struct VboData {
    void* data;
    size_t vertexCount;

    VboData() = default;
    VboData(const std::vector<Vertex>& vertices) {
        vertexCount = vertices.size();
        data = linearAlloc(vertexCount * sizeof(Vertex));
        memcpy(data, vertices.data(), vertexCount * sizeof(Vertex));
    }
};

class World;

class Chunk {
public:
    Chunk(World& world_, int uPosition_, s32 x_, s32 z_);
    ~Chunk() = default;

    void render();

    Block getBlock(s32 blockX, s32 blockY, s32 blockZ) const {
        return blocks[blockX][blockY][blockZ];
    }

    void setBlock(s32 blockX, s32 blockY, s32 blockZ, Block block) {
        blocks[blockX][blockY][blockZ] = block;
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
