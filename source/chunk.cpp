#include "chunk.hpp"

Chunk::Chunk(s32 x_, s32 z_, int uPosition_) : x{x_}, z{z_}, uPosition{uPosition_} {
    // HACK
    for (s32 blockZ = 0; blockZ < CHUNK_WIDTH; blockZ++) {
        for (s32 blockY = 0; blockY < CHUNK_HEIGHT; blockY++) {
            for (s32 blockX = 0; blockX < CHUNK_WIDTH; blockX++) {
                blocks[blockX][blockY][blockZ] = static_cast<BlockType>(1 + rand() % 3);
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
                BlockType block = blocks[blockX][blockY][blockZ];
                if (block == BlockType::None) continue;
                for (u8 face = 0; face < 6; face++) {
                    float texCoordOffset = enumToInt(blockTextures[enumToInt(block)][face]);
                    //std::cout << texCoordOffset << std::endl;
                    for (u8 v = 0; v < 6; v++) {
                        Vertex vertex = cubeVertices[face][v];
                        vertex.position.x += blockX;
                        vertex.position.y += blockY;
                        vertex.position.z += blockZ;
                        vertex.texCoord.x = texCoordOffset * textureSizeWithBorderNorm + vertex.texCoord.x * textureSizeNorm;
                        //std::cout << vertex.texCoord.x << std::endl;
                        vertices.push_back(vertex);
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
