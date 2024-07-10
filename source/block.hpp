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
};

const u8 textureCount = 5;
const u16 textureAtlasWidth = 128;
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
};

// PZ, NZ, PX, NX, PY, NY
constexpr TextureType blockTextures[][6] = {
    {TextureType::None, TextureType::None, TextureType::None, TextureType::None, TextureType::None, TextureType::None},
    {TextureType::Dirt, TextureType::Dirt, TextureType::Dirt, TextureType::Dirt, TextureType::Dirt, TextureType::Dirt},
    {TextureType::DirtGrass, TextureType::DirtGrass, TextureType::DirtGrass, TextureType::DirtGrass, TextureType::Grass, TextureType::Dirt},
    {TextureType::Stone, TextureType::Stone, TextureType::Stone, TextureType::Stone, TextureType::Stone, TextureType::Stone},
};
