#include "world.hpp"

const u8 MAX_ACTIVE_THREADS = 4 - 1;
const u16 THREAD_STACK_SIZE = 2 * 1024;

void generateChunk(void* chunk) {
    static_cast<Chunk*>(chunk)->generate();
}

World::World(Camera& camera_, int uPosition_) : camera{camera_}, uPosition{uPosition_} {
    chunks.reserve((RENDER_DISTANCE * 2 + 1) * (RENDER_DISTANCE * 2 + 1));

    for (s32 z = -TRACK_DISTANCE; z <= TRACK_DISTANCE; z++) {
        for (s32 x = -TRACK_DISTANCE; x <= TRACK_DISTANCE; x++) {
            chunks.emplace_back(*this, uPosition, x, z);
            chunks.back().generate();
        }
    }
}

void World::render() {
    findTrackedChunks();
    //updateQueues();

    for (s16 z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; z++) {
        for (s16 x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; x++) {
            // Only render chunks within the render distance
            // < is used instead of <= to prevent that one chunk on the edge from sticking out
            if ((x * x + z * z) < RENDER_DISTANCE * RENDER_DISTANCE) {
                Chunk* chunk = getTrackedChunk(x, z);
                if (chunk) {
                    chunk->render();
                }
            }
        }
    }
}

void World::moveCamera(const C3D_FVec& movement, bool& isOnGround, bool& wallJump) {
    s8 signs[3];
    signs[0] = movement.x > 0.0f ? 1 : -1;
    signs[1] = movement.y > 0.0f ? 1 : -1;
    signs[2] = movement.z > 0.0f ? 1 : -1;

    s32 cameraBlockX = std::floor(camera.aabb.position.x);
    s32 cameraBlockY = std::floor(camera.aabb.position.y);
    s32 cameraBlockZ = std::floor(camera.aabb.position.z);

    for (u8 axis = 0; axis < 3; axis++) {
        u8 trueAxis = 3 - axis;
        camera.aabb.position.c[trueAxis] += movement.c[trueAxis];
        AABB blockAABB;
        for (s32 x = -1; x <= 1; x++) {
            for (s32 y = -2; y <= 2; y++) {
                for (s32 z = -1; z <= 1; z++) {
                    s32 blockX = cameraBlockX + x;
                    s32 blockY = cameraBlockY + y;
                    s32 blockZ = cameraBlockZ + z;
                    if (blockY >= 0 && blockY < CHUNK_HEIGHT) {
                        Block block = getBlock(blockX, blockY, blockZ);
                        if (getBlockFlags(block.ty) & BlockFlags::Solid) {
                            blockAABB.position = C3D_FVec{.z = blockZ, .y = blockY, .x = blockX};
                            //std::cout << "Block at: " << blockAABB.position.x << ", " << blockAABB.position.y << ", " << blockAABB.position.z << std::endl;
                            if (camera.aabb.collidesWith(blockAABB)) {
                                camera.aabb.position.c[trueAxis] = blockAABB.position.c[trueAxis] - blockAABB.scale.c[trueAxis] / 2.0f * signs[axis] - camera.aabb.scale.c[trueAxis] / 2.0f * signs[axis];
                                if (axis == 1 && signs[axis] == -1) {
                                    isOnGround = true;
                                }
                                if (axis == 0 || axis == 2) {
                                    //if (blockY >= CHUNK_HEIGHT || !(getBlockFlags(getBlock(blockX, blockY + 1, blockZ).ty) & BlockFlags::Solid)) {
                                    wallJump = true;
                                    //}
                                }
                                //std::cout << "Collision at: " << blockAABB.position.x << ", " << blockAABB.position.y << ", " << blockAABB.position.z << std::endl;
                                //std::cout << "(Relative) Collision at: " << nBlockX << ", " << nBlockY << ", " << nBlockZ << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
}

void World::findTrackedChunks() {
    cameraChunkX = std::floor((float)camera.aabb.position.x / (float)CHUNK_WIDTH);
    cameraChunkZ = std::floor((float)camera.aabb.position.z / (float)CHUNK_WIDTH);

    for (u8 z = 0; z < TRACK_GRID_SIZE; z++) {
        for (u8 x = 0; x < TRACK_GRID_SIZE; x++) {
            trackedChunks[x][z] = nullptr;
        }
    }

    //activeThreads = 0;
    for (auto& chunk : chunks) {
        s32 chunkRelX = chunk.getX() - cameraChunkX;
        s32 chunkRelZ = chunk.getZ() - cameraChunkZ;
        if (std::abs(chunkRelX) <= TRACK_DISTANCE && std::abs(chunkRelZ) <= TRACK_DISTANCE) {
            getTrackedChunk(chunkRelX, chunkRelZ) = &chunk;
        } else {
            chunk.freeData();
        }

        // Check if the generation thread is still running
        //if (chunk.generationThreadIsRunning()) {
        //    activeThreads++;
        //}
    }

    for (s16 z = -TRACK_DISTANCE; z <= TRACK_DISTANCE; z++) {
        for (s16 x = -TRACK_DISTANCE; x <= TRACK_DISTANCE; x++) {
            Chunk*& chunk = getTrackedChunk(x, z);
            if (!chunk) {
                chunks.emplace_back(*this, uPosition, cameraChunkX + x, cameraChunkZ + z);
                chunk = &chunks[chunks.size() - 1];
                chunk->setGenerationThread(threadCreate(generateChunk, chunk, THREAD_STACK_SIZE, 0x18, -2, false));
                //generationQueue.push_back(chunk);
            }
        }
    }
}

//void World::updateQueues() {
//    for (;;) {
//        if (generationQueue.size() == 0/* || activeThreads >= MAX_ACTIVE_THREADS*/) {
//            break;
//        }
//
//        Chunk* chunk = generationQueue[0];
//        chunk->setGenerationThread(threadCreate(generateChunk, chunk, 512, 0x18, -2, false));
//        //chunk->generate();
//        generationQueue.erase(generationQueue.begin());
//        activeThreads++;
//    }
//}
