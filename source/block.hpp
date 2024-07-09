#pragma once

#include "common.hpp"

enum class TextureType {
    Dirt,
    DirtGrass,
    Grass,
    Sand,
};

enum class BlockType {
    None,
    Dirt,
    Grass,
    Sand,
};

struct Block {
    BlockType ty;
    TextureType textures[6];
};
