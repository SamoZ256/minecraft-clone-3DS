#include "chunk.hpp"

Chunk::Chunk(s32 x_, s32 z_, int uPosition_) : x{x_}, z{z_}, uPosition{uPosition_} {
    for (s32 blockZ = 0; blockZ < CHUNK_WIDTH; blockZ++) {
        for (s32 blockY = 0; blockY < CHUNK_HEIGHT; blockY++) {
            for (s32 blockX = 0; blockX < CHUNK_WIDTH; blockX++) {
                blocks[blockX][blockY][blockZ].ty = BlockType::Dirt;
            }
        }
    }
}

void Chunk::render() const {
    for (s32 blockZ = 0; blockZ < CHUNK_WIDTH; blockZ++) {
        for (s32 blockY = 0; blockY < CHUNK_HEIGHT; blockY++) {
            for (s32 blockX = 0; blockX < CHUNK_WIDTH; blockX++) {
                if (blocks[blockX][blockY][blockZ].ty == BlockType::None) continue;

                C3D_FVUnifSet(GPU_VERTEX_SHADER, uPosition, x * CHUNK_WIDTH + blockX, blockY, z * CHUNK_WIDTH + blockZ, 1.0f);

                // Draw
                C3D_DrawArrays(GPU_TRIANGLES, 0, 36);
            }
        }
    }
}
