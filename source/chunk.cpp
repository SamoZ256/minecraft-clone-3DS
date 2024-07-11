#include "chunk.hpp"

#include "world.hpp"

Chunk::Chunk(World& world_, int uPosition_, s32 x_, s32 z_) : world{world_}, uPosition{uPosition_}, x{x_}, z{z_} {
    // HACK
    //s32 a = -x -(z % 2);
    //if (a < 0) a = -a;
    for (s32 blockZ = 0; blockZ < CHUNK_WIDTH; blockZ++) {
        for (s32 blockY = 0; blockY < CHUNK_HEIGHT; blockY++) {
            for (s32 blockX = 0; blockX < CHUNK_WIDTH; blockX++) {
                if (blockY > CHUNK_HEIGHT - 2 + rand() % 2) continue;
                blocks[blockX][blockY][blockZ].ty = static_cast<BlockType>(1 + rand() % 2);
                if (blockX == 0 || blockX == CHUNK_WIDTH - 1 ||
                    blockZ == 0 || blockZ == CHUNK_WIDTH - 1)
                    blocks[blockX][blockY][blockZ].ty = BlockType::Stone;
            }
        }
    }
}

void Chunk::render() {
    if (!allocated) {
        allocate();
    }

    // Set position
    C3D_FVUnifSet(GPU_VERTEX_SHADER, uPosition, x * CHUNK_WIDTH, 0.0f, z * CHUNK_WIDTH, 1.0f);

    // Bind the VBO
    C3D_SetBufInfo(&vbo);

    // Draw
    C3D_DrawArrays(GPU_TRIANGLES, 0, vboData.vertexCount);
}

void Chunk::allocate() {
    std::vector<Vertex> vertices;
    vertices.reserve(2048);

    for (s32 blockZ = 0; blockZ < CHUNK_WIDTH; blockZ++) {
        for (s32 blockY = 0; blockY < CHUNK_HEIGHT; blockY++) {
            for (s32 blockX = 0; blockX < CHUNK_WIDTH; blockX++) {
                Block block = blocks[blockX][blockY][blockZ];
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
                        //std::cout << "B1: " << x << ", " << z << " : " << checkBlockX << ", " << checkBlockZ << std::endl;
                        checkBlock = world.getBlock(x * CHUNK_WIDTH + checkBlockX, checkBlockY, z * CHUNK_WIDTH + checkBlockZ);
                    }
                    /*if (checkBlockX < 0) {
                        checkBlock = world.getTrackedChunk(x - 1 - world.cameraChunkX, z - world.cameraChunkZ)->getBlock(CHUNK_WIDTH - 1, checkBlockY, checkBlockZ);
                    } else if (checkBlockX >= CHUNK_WIDTH) {
                        checkBlock = world.getTrackedChunk(x + 1 - world.cameraChunkX, z - world.cameraChunkZ)->getBlock(0, checkBlockY, checkBlockZ);
                    } else if (checkBlockZ < 0) {
                        checkBlock = world.getTrackedChunk(x - world.cameraChunkX, z - 1 - world.cameraChunkZ)->getBlock(checkBlockX, checkBlockY, CHUNK_WIDTH - 1);
                    } else if (checkBlockZ >= CHUNK_WIDTH) {
                        checkBlock = world.getTrackedChunk(x - world.cameraChunkX, z + 1 - world.cameraChunkZ)->getBlock(checkBlockX, checkBlockY, 0);
                    } else if (checkBlockY < 0 || checkBlockY >= CHUNK_WIDTH) {
                        checkBlock = Block(BlockType::None);
                    }*/ else {
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
	BufInfo_Init(&vbo);
	BufInfo_Add(&vbo, vboData.data, sizeof(Vertex), 3, 0x210);

    allocated = true;
}
