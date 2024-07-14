#include "world.hpp"

#include <limits>

#include "ray.hpp"

const u8 MAX_ACTIVE_THREADS = 4 - 1;
const u16 THREAD_STACK_SIZE = 2 * 1024;

void generateChunk(void* chunk) {
    static_cast<Chunk*>(chunk)->generate();
}

void World::initialize() {
    // Random seed
    if (seed == 0) {
        seed = (unsigned)time(NULL);
        if (seed == 0) {
            seed = 1;
        }
    }
    srand(seed);

    temperatureNoiseSeed = (rand() % 1000000) / 10000.0f;
    humidityNoiseSeed = (rand() % 1000000) / 10000.0f;
    noiseSeed = (rand() % 1000000) / 10000.0f;

    chunks.reserve((RENDER_DISTANCE * 2 + 1) * (RENDER_DISTANCE * 2 + 1));
    for (s32 z = -TRACK_DISTANCE; z <= TRACK_DISTANCE; z++) {
        for (s32 x = -TRACK_DISTANCE; x <= TRACK_DISTANCE; x++) {
            Chunk* chunk = new Chunk(*this, uPosition, x, z);
            chunks.push_back(chunk);
            chunk->generate();
        }
    }
}

void World::load(const void* loadData) {
    const u32* ptr = (const u32*)loadData;

    seed = *ptr;
    ptr++;
    u32 numModifiedChunks = *ptr;
    ptr++;
    for (u32 i = 0; i < numModifiedChunks; i++) {
        u64 chunkHash = *(u64*)ptr;
        ptr++;
        ptr++;
        u32 numModifiedBlocks = *ptr;
        ptr++;
        auto& data = modifiedBlocks[chunkHash];
        data.resize(numModifiedBlocks);
        memcpy(data.data(), ptr, numModifiedBlocks * sizeof(ModifiedBlock));
        ptr = (u32*)((u8*)ptr + numModifiedBlocks * sizeof(ModifiedBlock));
    }
}

void World::save(void*& saveData, u32& saveDataSize) const {
    // Find the size
    saveDataSize = sizeof(u32); // Seed
    saveDataSize += sizeof(u32); // Number of modified chunks
    for (auto [key, data] : modifiedBlocks) {
        if (data.size() != 0) {
            saveDataSize += sizeof(s32); // Chunk hash
            saveDataSize += sizeof(u32); // Number of modified blocks in the chunk
            saveDataSize += data.size() * sizeof(ModifiedBlock); // Modified blocks
        }
    }
    saveData = malloc(saveDataSize);

    // Copy the data
    u32* ptr = (u32*)saveData;

    *ptr = seed; // Seed
    ptr++;
    u32* numModifiedChunks = ptr; // Number of modified chunks
    *numModifiedChunks = 0;
    ptr++;
    for (auto [key, data] : modifiedBlocks) {
        if (data.size() != 0) {
            *numModifiedChunks += 1; // Number of modified chunks
            *(u64*)ptr = key; // Chunk hash
            ptr++;
            ptr++;
            *ptr = data.size(); // Number of modified blocks in the chunk
            ptr++;
            memcpy(ptr, data.data(), data.size() * sizeof(ModifiedBlock)); // Modified blocks
            ptr = (u32*)((u8*)ptr + data.size() * sizeof(ModifiedBlock));
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
                            blockAABB.position = float3(blockX, blockY, blockZ);
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

Intersection World::getIntersection() {
    Ray ray(camera.getPosition(), camera.direction);
    float minDist = std::numeric_limits<float>::max();

    s8 signs[3];
    signs[0] = camera.direction.x > 0.0f ? 1 : -1;
    signs[1] = camera.direction.y > 0.0f ? 1 : -1;
    signs[2] = camera.direction.z > 0.0f ? 1 : -1;

    s32 cameraBlockX = std::floor(camera.aabb.position.x);
    s32 cameraBlockY = std::floor(camera.aabb.position.y);
    s32 cameraBlockZ = std::floor(camera.aabb.position.z);

    Intersection intersection{};
    for (s32 x = -1; x <= BLOCK_BREAK_DIST; x++) {
        for (s32 y = -1; y <= BLOCK_BREAK_DIST; y++) {
            for (s32 z = -1; z <= BLOCK_BREAK_DIST; z++) {
                s32 relX = x * signs[0];
                s32 relY = y * signs[1];
                s32 relZ = z * signs[2];
                s32 blockX = cameraBlockX + relX;
                s32 blockY = cameraBlockY + relY;
                s32 blockZ = cameraBlockZ + relZ;
                if (blockY >= 0 && blockY < CHUNK_HEIGHT) {
                    Block block = getBlock(blockX, blockY, blockZ);
                    if (getBlockFlags(block.ty) & BlockFlags::Breakable) {
                        float dist = ray.intersectsCube(float3(blockX, blockY, blockZ));
                        if (dist > -1.0f && dist <= BLOCK_BREAK_DIST && dist < minDist) {
                            intersection.x = blockX;
                            intersection.y = blockY;
                            intersection.z = blockZ;
                            intersection.found = true;
                            minDist = dist;
                        }
                    }
                }
            }
        }
    }

    if (intersection.found) {
        // Get new block coordinates
        C3D_FVec diff = ray.origin + ray.direction * minDist - float3(intersection.x, intersection.y, intersection.z);
        //std::cout << diff.x << ", " << diff.y << ", " << diff.z << std::endl;

        float coordMax = fmax(fmax(abs(diff.x), abs(diff.y)), abs(diff.z));
        if (abs(diff.x) == coordMax) {
            intersection.placeNormal[0] = diff.x > 0.0f ? 1 : -1;
        } else if (abs(diff.y) == coordMax) {
            intersection.placeNormal[1] = diff.y > 0.0f ? 1 : -1;
        } else if (abs(diff.z) == coordMax) {
            intersection.placeNormal[2] = diff.z > 0.0f ? 1 : -1;
        }
    }

    return intersection;
}

void World::breakBlock(const Intersection& intersection) {
    setBlockType(intersection.x, intersection.y, intersection.z, BlockType::None);

    updateChunksAfterBlockChange(intersection.x, intersection.y, intersection.z, false);
}

void World::placeBlock(const Intersection& intersection, BlockType ty) {
    s32 x = intersection.x + intersection.placeNormal[0];
    s32 y = intersection.y + intersection.placeNormal[1];
    s32 z = intersection.z + intersection.placeNormal[2];
    setBlockType(x, y, z, ty);

    updateChunksAfterBlockChange(x, y, z, true);
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
    for (u32 i = 0; i < chunks.size(); i++) {
        Chunk*& chunk = chunks[i];
        s32 chunkRelX = chunk->getX() - cameraChunkX;
        s32 chunkRelZ = chunk->getZ() - cameraChunkZ;
        if (std::abs(chunkRelX) <= TRACK_DISTANCE && std::abs(chunkRelZ) <= TRACK_DISTANCE) {
            getTrackedChunk(chunkRelX, chunkRelZ) = chunk;
        } else {
            chunk->freeData();
            if (std::abs(chunkRelX) > DEALLOCATE_DISTANCE && std::abs(chunkRelZ) > DEALLOCATE_DISTANCE) {
                delete chunk;
                chunk = nullptr;
                freeChunkIndices.push_back(i);
            }
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
                chunk = new Chunk(*this, uPosition, cameraChunkX + x, cameraChunkZ + z);
                // Check if there are any free chunk slots
                if (freeChunkIndices.size() != 0) {
                    chunks[freeChunkIndices.back()] = chunk;
                    freeChunkIndices.pop_back();
                } else {
                    chunks.push_back(chunk);
                }
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

void World::updateChunksAfterBlockChange(s32 x, s32 y, s32 z, bool isPlace) {
    // Update chunks
    s32 chunkX = std::floor((float)x / (float)CHUNK_WIDTH);
    s32 chunkZ = std::floor((float)z / (float)CHUNK_WIDTH);
    s32 relChunkX = chunkX - cameraChunkX;
    s32 relChunkZ = chunkZ - cameraChunkZ;

    Chunk* chunk = getTrackedChunk(relChunkX, relChunkZ);
    if (chunk) {
        chunk->freeData();
    }

    // TODO: clean this up
    // Update other chunks if on the edge of a chunk
    s32 relX = x - chunkX * CHUNK_WIDTH;
    s32 relZ = z - chunkZ * CHUNK_WIDTH;

#define SHOULD_UPDATE_CHUNK (!isPlace && (block.ty != BlockType::None || getBlockFlags(chunk->getBlock(relX, y, relZ).ty) & BlockFlags::Transparent))

    if (relX == 0) {
        Chunk* checkChunk = getTrackedChunk(relChunkX - 1, relChunkZ);
        if (checkChunk) {
            Block block = checkChunk->getBlock(CHUNK_WIDTH - 1, y, relZ);
            if (SHOULD_UPDATE_CHUNK) {
                checkChunk->freeData();
            }
        }
    } else if (relX == CHUNK_WIDTH - 1) {
        Chunk* checkChunk = getTrackedChunk(relChunkX + 1, relChunkZ);
        if (checkChunk) {
            Block block = checkChunk->getBlock(0, y, relZ);
            if (SHOULD_UPDATE_CHUNK) {
                checkChunk->freeData();
            }
        }
    }
    if (relZ == 0) {
        Chunk* checkChunk = getTrackedChunk(relChunkX, relChunkZ - 1);
        if (checkChunk) {
            Block block = checkChunk->getBlock(CHUNK_WIDTH - 1, y, relZ);
            if (SHOULD_UPDATE_CHUNK) {
                checkChunk->freeData();
            }
        }
    } else if (relZ == CHUNK_WIDTH - 1) {
        Chunk* checkChunk = getTrackedChunk(relChunkX, relChunkZ + 1);
        if (checkChunk) {
            Block block = checkChunk->getBlock(CHUNK_WIDTH - 1, y, relZ);
            if (SHOULD_UPDATE_CHUNK) {
                checkChunk->freeData();
            }
        }
    }

#undef SHOULD_UPDATE_CHUNK
}
