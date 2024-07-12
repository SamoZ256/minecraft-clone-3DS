#include "world.hpp"

const u8 MAX_ACTIVE_THREADS = 2;

void generateChunk(void* chunk) {
    static_cast<Chunk*>(chunk)->generate();
}

World::World(const Camera& camera_, int uPosition_) : camera{camera_}, uPosition{uPosition_} {
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
    updateQueues();

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

void World::findTrackedChunks() {
    cameraChunkX = std::floor((float)camera.position.x / (float)CHUNK_WIDTH);
    cameraChunkZ = std::floor((float)camera.position.z / (float)CHUNK_WIDTH);

    for (u8 z = 0; z < TRACK_GRID_SIZE; z++) {
        for (u8 x = 0; x < TRACK_GRID_SIZE; x++) {
            trackedChunks[x][z] = nullptr;
        }
    }

    for (auto& chunk : chunks) {
        s32 chunkRelX = chunk.getX() - cameraChunkX;
        s32 chunkRelZ = chunk.getZ() - cameraChunkZ;
        if (std::abs(chunkRelX) <= TRACK_DISTANCE && std::abs(chunkRelZ) <= TRACK_DISTANCE) {
            getTrackedChunk(chunkRelX, chunkRelZ) = &chunk;
        } else {
            chunk.freeData();
        }

        // Decrement cative threads if finished
        if (chunk.didJustFinishGeneration()) {
            activeThreads--;
        }
    }

    for (s16 z = -TRACK_DISTANCE; z <= TRACK_DISTANCE; z++) {
        for (s16 x = -TRACK_DISTANCE; x <= TRACK_DISTANCE; x++) {
            Chunk*& chunk = getTrackedChunk(x, z);
            if (!chunk) {
                chunks.emplace_back(*this, uPosition, cameraChunkX + x, cameraChunkZ + z);
                chunk = &chunks.back();
            }
            if (!chunk->isGenerated()) {
                generationQueue.push_back(chunk);
            }
        }
    }
}

void World::updateQueues() {
    // HACK
    for (u32 i = 0; i < MAX_ACTIVE_THREADS - 1; i++) {
        if (generationQueue.size() == 0 || activeThreads >= (MAX_ACTIVE_THREADS - 1)) {
            break;
        }

        Chunk* chunk = generationQueue[0];
        chunk->setGenerationThread(threadCreate(generateChunk, chunk, 16 * 1024, 0x3F, -2, false));
        generationQueue.erase(generationQueue.begin());
        activeThreads++;
    }
    generationQueue.clear();
}
