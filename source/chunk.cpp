#include "chunk.hpp"

#define DB_PERLIN_IMPL
#include "db_perlin.hpp"

#include "world.hpp"

const double NOISE_FREQ = 0.04;

void Chunk::generate() {
    //if (generated) {
    //    return;
    //}

    for (s32 blockZ = 0; blockZ < CHUNK_WIDTH; blockZ++) {
        for (s32 blockX = 0; blockX < CHUNK_WIDTH; blockX++) {
            const double terrainBias = 1.4; // The higher the value the more flat the terrain is
            const double terrainYOffset = 0.0; // Higher values will make the biome deeper, lower values will make it taller

            // Number of blocks in a row
            u8 groundBlockCount = 0;
            for (s32 blockY = CHUNK_HEIGHT - 1; blockY >= 0; blockY--) {
                double noise = db::perlin((x * CHUNK_WIDTH + blockX) * NOISE_FREQ, blockY * NOISE_FREQ, (z * CHUNK_WIDTH + blockZ) * NOISE_FREQ);

                // Exponential ease
                double yNorm = (double)blockY / CHUNK_HEIGHT + terrainYOffset;
                double shift = yNorm <= 0.5 ? pow(yNorm * 2.0, terrainBias) / 2.0 : 1.0 - pow((1.0 - yNorm) * 2.0, terrainBias) / 2.0;
                shift = shift * 2.0 - 1.0;
                // TODO: apply the shift differently?
                noise += shift;

                if (noise < 0.0) {
                    groundBlockCount++;

                    BlockType& blockTy = blocks[blockX][blockY][blockZ].ty;
                    if (groundBlockCount == 1) {
                        blockTy = BlockType::Grass;

                        // Generate a tree
                        if (rand() % 128 == 0) {
                            // Trunk
                            for (s32 i = 0; i < 4 + rand() % 3; i++) {
                                setBlockTypeChecked(blockX, blockY + 1 + i, blockZ, {BlockType::Wood});
                            }

                            // Leaves
                            for (s32 i = 0; i < 3; i++) {
                                for (s32 j = -1; j <= 1; j++) {
                                    for (s32 k = -1; k <= 1; k++) {
                                        setBlockTypeChecked(blockX + j, blockY + 4 + i, blockZ + k, BlockType::Leaves);
                                    }
                                }
                            }
                        }
                    } else if (groundBlockCount <= 2 + rand() % 3) {
                        blockTy = BlockType::Dirt;
                    } else {
                        blockTy = BlockType::Stone;
                    }

                    // HACK: for debugging
                    if (blockX == 0 || blockX == CHUNK_WIDTH - 1 ||
                        blockZ == 0 || blockZ == CHUNK_WIDTH - 1) {
                        blockTy = BlockType::Stone;
                    }
                } else {
                    groundBlockCount = 0;
                }
            }
        }
    }

    generated = true;
}

void Chunk::render() {
    if (!allocated) {
        allocate();
    }

    // Set position
    C3D_FVec position{.z = z * CHUNK_WIDTH, 0.0f, x * CHUNK_WIDTH};
    position = FVec3_Subtract(position, world.getCamera().getPosition());
    C3D_FVUnifSet(GPU_VERTEX_SHADER, uPosition, position.x, position.y, position.z, 1.0f);

    // Bind the VBO
    C3D_SetBufInfo(&vbo);

    // Draw
    C3D_DrawElements(GPU_TRIANGLES, vboData.indexCount, C3D_UNSIGNED_SHORT, vboData.indexData);
}

void Chunk::freeData() {
    if (allocated) {
        vboData.freeData();
        allocated = false;
    }
}

void Chunk::allocate() {
    joinGenerationThread();

    std::vector<Vertex> vertices;
    vertices.reserve(1024);

    std::vector<u16> indices;
    indices.reserve(1536);

    u16 facesPushed = 0;
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

                    if (getBlockFlags(checkBlock.ty) & BlockFlags::Transparent && block.ty != checkBlock.ty) {
                        for (u8 v = 0; v < 4; v++) {
                            Vertex vertex = cubeVertices[face][v];
                            vertex.position[0] += blockX;
                            vertex.position[1] += blockY;
                            vertex.position[2] += blockZ;
                            vertex.texCoord.u = texCoordOffset * textureSizeWithBorderNorm + vertex.texCoord.u * textureSizeNorm;
                            vertices.push_back(vertex);
                        }
                        for (u8 i = 0; i < 6; i++) {
                            u16 index = faceIndices[i];
                            indices.push_back(facesPushed * 4 + index);
                        }
                        facesPushed++;
                    }
                }
            }
        }
    }

    // Allocate VBO
    vboData = VboData(vertices, indices);
	BufInfo_Init(&vbo);
	BufInfo_Add(&vbo, vboData.vertexData, sizeof(Vertex), 3, 0x210);

    allocated = true;
}
