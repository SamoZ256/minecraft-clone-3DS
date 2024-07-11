#pragma once

#include "common.hpp"

template<typename T>
int enumToInt(T e) {
    return static_cast<int>(e);
}

enum class TextureType {
    None,
    Dirt,
    DirtGrass,
    Grass,
    Stone,
    WoodSide,
    WoodTop,
    Leaves,
};

const u16 textureAtlasWidth = 256;
const u8 textureSize = 16;
// Textures have 1 pixel border on each side to prevent sampling to the other textures
const u8 textureSizeWithBorder = textureSize + 2;
const float textureSizeNorm = (float)textureSize / textureAtlasWidth;
const float textureSizeWithBorderNorm = (float)textureSizeWithBorder / textureAtlasWidth;

enum class BlockType {
    None,
    Dirt,
    Grass,
    Stone,
    Wood,
    Leaves,
};

struct Block {
    BlockType ty = BlockType::None;

    Block() = default;
    Block(BlockType ty_) : ty(ty_) {}
};

#define BLOCK_TEXTURES_ALL(textureTypeAll) \
    {textureTypeAll, textureTypeAll, textureTypeAll, textureTypeAll, textureTypeAll, textureTypeAll}

#define BLOCK_TEXTURES_SIDES_TOP(textureTypeSides, textureTypeTop) \
    {textureTypeSides, textureTypeSides, textureTypeSides, textureTypeSides, textureTypeTop, textureTypeTop}

#define BLOCK_TEXTURES_SIDES_TOP_BOTTOM(textureTypeSides, textureTypeTop, textureTypeBottom) \
    {textureTypeSides, textureTypeSides, textureTypeSides, textureTypeSides, textureTypeTop, textureTypeBottom}

// PZ, NZ, PX, NX, PY, NY
constexpr TextureType blockTextures[][6] = {
    BLOCK_TEXTURES_ALL(TextureType::None),
    BLOCK_TEXTURES_ALL(TextureType::Dirt),
    BLOCK_TEXTURES_SIDES_TOP_BOTTOM(TextureType::DirtGrass, TextureType::Grass, TextureType::Dirt),
    BLOCK_TEXTURES_ALL(TextureType::Stone),
    BLOCK_TEXTURES_SIDES_TOP(TextureType::WoodSide, TextureType::WoodTop),
    BLOCK_TEXTURES_ALL(TextureType::Leaves),
};
