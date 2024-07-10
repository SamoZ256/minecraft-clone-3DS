#include "chunk.hpp"

#include "world.hpp"

Chunk::Chunk(World& world_, int uPosition_, s32 x_, s32 z_) : world{world_}, uPosition{uPosition_}, x{x_}, z{z_} {
    // HACK
    for (s32 blockZ = 0; blockZ < CHUNK_WIDTH; blockZ++) {
        for (s32 blockY = 0; blockY < CHUNK_HEIGHT; blockY++) {
            for (s32 blockX = 0; blockX < CHUNK_WIDTH; blockX++) {
                blocks[blockX][blockY][blockZ].ty = static_cast<BlockType>(1 + rand() % 3);
            }
        }
    }
}

void Chunk::render() {
    if (!allocated) {
        allocate();
    }

    // Set position
    C3D_FVUnifSet(GPU_VERTEX_SHADER, uPosition, x * CHUNK_WIDTH , 0.0f, z * CHUNK_WIDTH, 1.0f);

    // Bind the VBO
    C3D_SetBufInfo(vbo);

    // Draw
    C3D_DrawArrays(GPU_TRIANGLES, 0, vboData.vertexCount);
}

void Chunk::allocate() {
    std::vector<Vertex> vertices;
    vertices.reserve(1024);

    for (s32 blockZ = 0; blockZ < CHUNK_WIDTH; blockZ++) {
        for (s32 blockY = 0; blockY < CHUNK_HEIGHT; blockY++) {
            for (s32 blockX = 0; blockX < CHUNK_WIDTH; blockX++) {
                const Block& block = blocks[blockX][blockY][blockZ];
                if (block.ty == BlockType::None) continue;
                for (u8 face = 0; face < 6; face++) {
                    float texCoordOffset = enumToInt(blockTextures[enumToInt(block.ty)][face]);

                    s32 checkBlockX = blockX;
                    s32 checkBlockY = blockY;
                    s32 checkBlockZ = blockZ;
                    switch (face) {
                    case 0: checkBlockZ++; break;
                    case 1: checkBlockZ--; break;
                    case 2: checkBlockX++; break;
                    case 3: checkBlockX--; break;
                    case 4: checkBlockY++; break;
                    case 5: checkBlockY--; break;
                    }

                    Block checkBlock;
                    // If the check block is outside of the chunk, check for the block globally
                    if (checkBlockX < 0 || checkBlockX >= CHUNK_WIDTH ||
                        checkBlockY < 0 || checkBlockY >= CHUNK_HEIGHT ||
                        checkBlockZ < 0 || checkBlockZ >= CHUNK_WIDTH) {
                        checkBlock = world.getBlock(x * CHUNK_WIDTH + checkBlockX, checkBlockY, z * CHUNK_WIDTH + checkBlockZ);
                    } else {
                        checkBlock = blocks[checkBlockX][checkBlockY][checkBlockZ];
                    }

                    if (checkBlock.ty == BlockType::None) {
                        for (u8 v = 0; v < 6; v++) {
                            Vertex vertex = cubeVertices[face][v];
                            vertex.position.x += blockX;
                            vertex.position.y += blockY;
                            vertex.position.z += blockZ;
                            vertex.texCoord.x = texCoordOffset * textureSizeWithBorderNorm + vertex.texCoord.x * textureSizeNorm;
                            vertices.push_back(vertex);
                        }
                    }
                }
            }
        }
    }

    // Allocate VBO
    vboData = VboData(vertices);
	vbo = C3D_GetBufInfo();
	BufInfo_Init(vbo);
	BufInfo_Add(vbo, vboData.data, sizeof(Vertex), 3, 0x210);

    allocated = true;
}
