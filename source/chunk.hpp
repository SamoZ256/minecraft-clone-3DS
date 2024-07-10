#pragma once

#include "block.hpp"
#include "vertex.hpp"

const u8 CHUNK_WIDTH = 8;
const u8 CHUNK_HEIGHT = 8;

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

class Chunk {
public:
    Chunk(s32 x_, s32 z_, int uPosition_);
    ~Chunk() = default;

    void render();

    Block getBlock(u8 xblockX, u8 xblockY, u8 xblockZ) const {
        return blocks[xblockX][xblockY][xblockZ];
    }

    void setBlock(u8 xblockX, u8 xblockY, u8 xblockZ, Block block) {
        blocks[xblockX][xblockY][xblockZ] = block;
    }

private:
    Block blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_WIDTH];
    s32 x, z;
    int uPosition;

    bool allocated = false;
    VboData vboData;
    C3D_BufInfo* vbo;

    void allocate();
};
