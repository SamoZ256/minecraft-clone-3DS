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
                if (blocks[blockX][blockY][blockZ].ty == BlockType::None) continue;
                for (u8 face = 0; face < 6; face++) {
                    for (u8 v = 0; v < 6; v++) {
                        Vertex vertex = cubeVertices[face][v];
                        vertex.position[0] += blockX;
                        vertex.position[1] += blockY;
                        vertex.position[2] += blockZ;
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
