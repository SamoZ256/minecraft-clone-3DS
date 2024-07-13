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
    Sand,
    RedFlower,
    YellowFlower,
    CactusSide,
    // TODO: add cactus bottom
    CactusTop,
};

const u16 TEXTURE_ATLAS_WIDTH = 256;
const u8 TEXTURE_SIZE = 16;
// Textures have 1 pixel border on each side to prevent sampling to the other textures
const u8 TEXTURE_SIZE_WITH_BORDER = TEXTURE_SIZE + 2;
const float TEXTURE_SIZE_NORM = (float)TEXTURE_SIZE / TEXTURE_ATLAS_WIDTH;
const float TEXTURE_SIZE_WITH_BORDER_NORM = (float)TEXTURE_SIZE_WITH_BORDER / TEXTURE_ATLAS_WIDTH;

enum class BlockType {
    None,
    Dirt,
    Grass,
    Stone,
    Wood,
    Leaves,
    Sand,
    RedFlower,
    YellowFlower,
    Cactus,
};

struct Block {
    BlockType ty = BlockType::None;

    Block() = default;
    Block(BlockType ty_) : ty(ty_) {}
};

enum class BlockFlags {
    None = 0x0,
    Solid = 0x1,
    Transparent = 0x2,
    Breakable = 0x4,
    ShapeCross = 0x8,
};

inline BlockFlags operator|(BlockFlags a, BlockFlags b) {
    return static_cast<BlockFlags>(enumToInt(a) | enumToInt(b));
}

inline int operator&(BlockFlags a, BlockFlags b) {
    return enumToInt(a) & enumToInt(b);
}

inline BlockFlags operator~(BlockFlags a) {
    return static_cast<BlockFlags>(~enumToInt(a));
}

const BlockFlags blockFlags[] = {
    BlockFlags::Transparent,
    BlockFlags::Solid | BlockFlags::Breakable,
    BlockFlags::Solid | BlockFlags::Breakable,
    BlockFlags::Solid | BlockFlags::Breakable,
    BlockFlags::Solid | BlockFlags::Breakable,
    BlockFlags::Solid | BlockFlags::Transparent | BlockFlags::Breakable,
    BlockFlags::Solid | BlockFlags::Transparent | BlockFlags::Breakable,
    BlockFlags::Transparent | BlockFlags::Breakable | BlockFlags::ShapeCross,
    BlockFlags::Transparent | BlockFlags::Breakable | BlockFlags::ShapeCross,
    BlockFlags::Solid | BlockFlags::Transparent | BlockFlags::Breakable,
};

inline BlockFlags getBlockFlags(BlockType ty) {
    return blockFlags[enumToInt(ty)];
}

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
    BLOCK_TEXTURES_ALL(TextureType::Sand),
    BLOCK_TEXTURES_ALL(TextureType::RedFlower),
    BLOCK_TEXTURES_ALL(TextureType::YellowFlower),
    BLOCK_TEXTURES_SIDES_TOP_BOTTOM(TextureType::CactusSide, TextureType::CactusTop, TextureType::CactusTop), // TODO: use cactus bottom
};
