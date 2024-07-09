#pragma once

#include "block.hpp"

const u8 CHUNK_WIDTH = 2;
const u8 CHUNK_HEIGHT = 2;

class Chunk {
public:
    Chunk(s32 x_, s32 z_, int uPosition_);
    ~Chunk() = default;

    void render() const;

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
};
